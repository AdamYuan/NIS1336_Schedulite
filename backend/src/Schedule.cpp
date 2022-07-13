#include <backend/Schedule.hpp>

#include <backend/Encryption.hpp>
#include <backend/Environment.hpp>

#include <condition_variable>

#include <ghc/filesystem.hpp>
#include <libipc/mutex.h>
#include <libipc/shm.h>

namespace backend {

struct Schedule::SyncObject {
	static constexpr const char *kIPCMutexHeader = "__SCHEDULITE_SCHEDULE_MUTEX__";
	static constexpr const char *kIPCSHMHeader = "__SCHEDULITE_SCHEDULE_SHM__";

	// Named IPC mutex
	ipc::sync::mutex ipc_mutex;

	// Shared memory
	ipc::shm::id_t shm_id{};
	uint32_t *shared_size{}, *shared_version{};
	unsigned char *shared_data{};

	explicit SyncObject(std::string_view identifier)
	    : ipc_mutex(std::string{kIPCMutexHeader + std::string(identifier)}.c_str()) {}
	~SyncObject() {
		if (shm_id) {
			ipc::shm::release(shm_id);
		}
	}
};

Schedule::Schedule(const std::shared_ptr<User> &user_ptr) {
	m_user_ptr = user_ptr;
	m_file_path = ghc::filesystem::path{m_user_ptr->GetInstancePtr()->GetScheduleDirPath()}
	                  .append(m_user_ptr->GetName())
	                  .string();
	m_sync_object = std::make_unique<SyncObject>(m_user_ptr->GetIdentifier());
}

std::tuple<std::shared_ptr<Schedule>, Error> Schedule::Acquire(const std::shared_ptr<User> &user_ptr) {
	std::shared_ptr<Schedule> ret = std::make_shared<Schedule>(user_ptr);
	Error error = ret->initialize_shm_locked();
	if (error != Error::kSuccess)
		return {nullptr, error};
	return {std::move(ret), Error::kSuccess};
}

std::tuple<uint32_t, Error> Schedule::TaskInsert(const TaskProperty &task_property) {
	std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
	// Load shared tasks
	std::vector<Task> tasks = load_tasks_from_shm();
	uint32_t id;
	{
		// fetch a unique id
		uint32_t max_id = 0;
		for (auto &t : tasks)
			max_id = std::max(t.id, max_id);
		id = max_id + 1;
		Error error = insert(&tasks, {id, task_property});
		if (error != Error::kSuccess)
			return {0, error};
	}
	return {id, store_tasks(tasks)};
}

Error Schedule::TaskErase(uint32_t id) {
	std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
	// Load shared tasks
	std::vector<Task> tasks = load_tasks_from_shm();
	{
		// find task
		auto it = std::find_if(tasks.begin(), tasks.end(), [&id](const Task &c) { return c.id == id; });
		if (it == tasks.end())
			return Error::kTaskNotFound;
		tasks.erase(it);
	}
	return store_tasks(tasks);
}

Error Schedule::TaskToggleDone(uint32_t id) {
	std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
	// Load shared tasks
	std::vector<Task> tasks = load_tasks_from_shm();
	{
		// find task
		auto it = std::find_if(tasks.begin(), tasks.end(), [&id](const Task &c) { return c.id == id; });
		if (it == tasks.end())
			return Error::kTaskNotFound;
		it->property.done ^= 1;
	}
	return store_tasks(tasks);
}

Error Schedule::TaskEdit(uint32_t id, const TaskProperty &property, TaskPropertyMask property_edit_mask) {
	if (property_edit_mask == TaskPropertyMask::kNone)
		return Error::kSuccess;

	std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
	// Load shared tasks
	std::vector<Task> tasks = load_tasks_from_shm();
	{
		// find task
		auto it = std::find_if(tasks.begin(), tasks.end(), [&id](const Task &c) { return c.id == id; });
		if (it == tasks.end())
			return Error::kTaskNotFound;

		Task task = TaskPatch(*it, property, property_edit_mask);
		if ((property_edit_mask & TaskPropertyMask::kKey) != TaskPropertyMask::kNone) {
			tasks.erase(it);
			Error error = insert(&tasks, task);
			if (error != Error::kSuccess)
				return error;
		} else
			*it = task;
	}
	return store_tasks(tasks);
}

const std::vector<Task> &Schedule::GetTasks() const { return GetTasks(nullptr); }
const std::vector<Task> &Schedule::GetTasks(bool *p_updated) const {
	// Acquire a local tasks cache
	std::unique_lock cache_lock{m_local_tasks_mutex};
	auto &local_tasks = m_local_tasks[std::this_thread::get_id()];
	cache_lock.unlock();

	{ // Examine the shared version
		std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
		if (*m_sync_object->shared_version > local_tasks.second) {
			local_tasks = {load_tasks_from_shm(), *m_sync_object->shared_version};
			if (p_updated)
				*p_updated = true;
		} else if (p_updated)
			*p_updated = false;
	}
	return local_tasks.first;
}

Error Schedule::insert(std::vector<Task> *tasks, const Task &task) {
	auto it = std::lower_bound(tasks->begin(), tasks->end(), task, TaskKeyLess);
	if (it != tasks->end() && TaskKeyEqual(task, *it))
		return Error::kTaskAlreadyExist;
	tasks->insert(it, task);
	return Error::kSuccess;
}

Error Schedule::initialize_shm_locked() {
	if (!m_user_ptr->GetInstancePtr()->MaintainDirs())
		return Error::kFileIOError;

	std::string shm_name = SyncObject::kIPCSHMHeader + m_user_ptr->GetIdentifier();
	{
		std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
		m_sync_object->shm_id = ipc::shm::acquire(shm_name.c_str(), kMaxSharedScheduleMemory + 8, ipc::shm::open);
		if (m_sync_object->shm_id) {
			// If SHM already exists, open it
			std::size_t size;
			auto mem = (unsigned char *)ipc::shm::get_mem(m_sync_object->shm_id, &size);
			if (size < kMaxSharedScheduleMemory + 8)
				return Error::kSHMInitializationError;
			m_sync_object->shared_size = (uint32_t *)mem;
			m_sync_object->shared_version = (uint32_t *)(mem + 4);
			m_sync_object->shared_data = mem + 8;
			return Error::kSuccess;
		} else {
			// Otherwise, create SHM and copy file data to it
			m_sync_object->shm_id = ipc::shm::acquire(shm_name.c_str(), kMaxSharedScheduleMemory + 8, ipc::shm::create);
			if (!m_sync_object->shm_id)
				return Error::kSHMInitializationError;
			std::size_t size;
			auto mem = (unsigned char *)ipc::shm::get_mem(m_sync_object->shm_id, &size);
			if (size < kMaxSharedScheduleMemory + 8)
				return Error::kSHMInitializationError;
			m_sync_object->shared_size = (uint32_t *)mem;
			m_sync_object->shared_version = (uint32_t *)(mem + 4);
			m_sync_object->shared_data = mem + 8;

			*m_sync_object->shared_size = 0;
			*m_sync_object->shared_version = 1;

			std::string encrypted;
			{
				ghc::filesystem::ifstream in{m_file_path, std::ios::binary};
				if (!in.is_open()) {
					return Error::kSuccess;
				}
				// get length of file
				in.seekg(0, ghc::filesystem::ifstream::end);
				std::streamsize length = in.tellg();
				in.seekg(0, ghc::filesystem::ifstream::beg);
				// read file
				if (length > 0) {
					encrypted.resize(length);
					in.read((char *)encrypted.data(), length);
				} else
					return Error::kSuccess;
			}
			std::string raw = Decrypt(encrypted, m_user_ptr->GetKey());
			if (raw.size() > kMaxSharedScheduleMemory)
				raw.resize(kMaxSharedScheduleMemory);
			*m_sync_object->shared_size = raw.size();
			*m_sync_object->shared_version = 1;
			std::copy(raw.begin(), raw.end(), m_sync_object->shared_data);
		}
	}
	return Error::kSuccess;
}

std::vector<Task> Schedule::load_tasks_from_shm() const {
	return parse_string({(char *)m_sync_object->shared_data, *m_sync_object->shared_size});
}

Error Schedule::store_tasks(const std::vector<Task> &tasks) {
	std::string raw = get_string(tasks);
	{ // Store to SHM
		if (raw.size() > kMaxSharedScheduleMemory) {
			return Error::kSHMSizeExceed;
		}
		*m_sync_object->shared_size = raw.size();
		++(*m_sync_object->shared_version);
		std::copy(raw.begin(), raw.end(), m_sync_object->shared_data);
	}

	{ // Then store to file
		if (!m_user_ptr->GetInstancePtr()->MaintainDirs())
			return Error::kFileIOError;
		std::string encrypted = Encrypt(raw, m_user_ptr->GetKey());
		ghc::filesystem::ofstream out{m_file_path, std::ios::binary};
		if (!out.is_open())
			return Error::kFileIOError;
		out.write(encrypted.data(), (std::streamsize)encrypted.size());
	}
	return Error::kSuccess;
}

std::string Schedule::get_string(const std::vector<Task> &tasks) {
	std::string ret = kStringHeader;
	for (const Task &task : tasks)
		ret += StrFromTask(task);
	return ret;
}
std::vector<Task> Schedule::parse_string(std::string_view str) {
	if (str.length() < kStringHeaderLength || str.substr(0, kStringHeaderLength) != kStringHeader)
		return std::vector<Task>{}; // Return empty if header not match (do not drop error)

	std::vector<Task> ret;
	str = str.substr(kStringHeaderLength);

	Task task;
	uint32_t len;
	while (true) {
		std::tie(task, len) = TaskFromStr(str);
		if (len == 0)
			break;
		ret.push_back(task);
		str = str.substr(len);
	}
	return ret;
}

} // namespace backend
