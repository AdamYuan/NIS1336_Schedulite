#include <backend/Schedule.hpp>

#include <backend/Encryption.hpp>

#include <ghc/filesystem.hpp>
#include <libipc/mutex.h>
#include <readerwriterqueue.h>

namespace backend {

struct Schedule::SyncObject {
	static constexpr const char *kIPCMutexHeader = "__SCHEDULITE_SCHEDULE_MUTEX__";
	ipc::sync::mutex ipc_mutex;
	moodycamel::BlockingReaderWriterQueue<Operation> operation_queue;
	explicit SyncObject(std::string_view username)
	    : ipc_mutex(std::string{kIPCMutexHeader + std::string(username)}.c_str()) {}
	~SyncObject() = default;
};

Schedule::Schedule(const std::shared_ptr<User> &user_ptr) {
	m_user_ptr = user_ptr;
	m_file_path =
	    ghc::filesystem::path{m_user_ptr->GetInstanceSPtr()->GetUserDirPath()}.append(m_user_ptr->GetName()).string();
	m_sync_object = std::make_shared<SyncObject>(m_user_ptr->GetName());
}

std::tuple<std::shared_ptr<Schedule>, Error> Schedule::Create(const std::shared_ptr<User> &user_ptr, bool create_file) {
	std::shared_ptr<Schedule> ret = std::make_shared<Schedule>(user_ptr);
	Error error;
	if (create_file) {
		if ((error = ret->create_file()) != Error::kSuccess)
			return {nullptr, error};
	}

	std::vector<Task> tasks;
	std::tie(tasks, error) = ret->load_tasks(true);
	ret->push_sync_tasks(std::move(tasks));

	if (error != Error::kSuccess)
		return {nullptr, error};
	ret->m_operation_thread = std::thread{&Schedule::operation_thread_func, ret.get()};
	ret->m_sync_thread = std::thread{&Schedule::sync_thread_func, ret.get()};
	return {std::move(ret), Error::kSuccess};
}

Schedule::~Schedule() {
	m_thread_run.store(false, std::memory_order_release);

	m_sync_object->operation_queue.enqueue({Operation::kQuit});

	if (m_sync_thread.joinable())
		m_sync_thread.join();
	if (m_operation_thread.joinable())
		m_operation_thread.join();
}

Error Schedule::operate(std::vector<Task> *tasks, const Schedule::Operation &operation) {
	if (operation.op == Operation::kInsert) {
		// insert
		uint32_t max_id = 0;
		for (auto &task : *tasks)
			max_id = std::max(task.id, max_id);

		auto task = operation.task;
		task.id = max_id + 1;

		auto it = std::lower_bound(tasks->begin(), tasks->end(), task);
		if (it != tasks->end() && task == *it)
			return Error::kTaskAlreadyExist;
		tasks->insert(it == tasks->end() ? it : it + 1, task);
	} else {
		// erase
		uint32_t id = operation.task.id;
		auto it = std::find_if(tasks->begin(), tasks->end(), [&id](const Task &c) { return c.id == id; });
		if (it == tasks->end())
			return Error::kTaskNotFound;
		tasks->erase(it);
	}
	return Error::kSuccess;
}

std::future<Error> Schedule::Insert(std::string_view name, TimeInt begin_time, TimeInt remind_time,
                                    Schedule::Priority priority, Schedule::Type type) {
	std::promise<Error> promise;
	auto ret = promise.get_future();
	m_sync_object->operation_queue.enqueue(
	    {Operation::kInsert, {0, std::string{name}, begin_time, remind_time, priority, type}, std::move(promise)});
	return ret;
}

std::future<Error> Schedule::Erase(uint32_t id) {
	std::promise<Error> promise;
	auto ret = promise.get_future();
	m_sync_object->operation_queue.enqueue({Operation::kErase, {id}, std::move(promise)});
	return ret;
}

Error Schedule::create_file() {
	if (!m_user_ptr->GetInstanceSPtr()->MaintainDirs())
		return Error::kFileIOError;

	{
		std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};

		{
			std::ifstream in{m_file_path};
			if (in.is_open())
				return Error::kUserAlreadyExist;
		}
		std::ofstream out{m_file_path};
		if (!out.is_open())
			return Error::kFileIOError;
		std::string encrypted = Encrypt(kStringHeader, m_user_ptr->GetKey());
		out.write(encrypted.data(), (std::streamsize)encrypted.size());
	}
	return Error::kSuccess;
}

std::tuple<std::vector<Schedule::Task>, Error> Schedule::load_tasks(bool lock) {
	if (!m_user_ptr->GetInstanceSPtr()->MaintainDirs())
		return {std::vector<Task>{}, Error::kFileIOError};

	std::string encrypted;
	if (lock) {
		std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
		std::ifstream in{m_file_path};
		if (!in.is_open())
			return {std::vector<Task>{}, Error::kUserNotFound};
		encrypted = {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	} else {
		std::ifstream in{m_file_path};
		if (!in.is_open())
			return {std::vector<Task>{}, Error::kUserNotFound};
		encrypted = {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	}

	auto [ret, error] = parse_string(Decrypt(encrypted, m_user_ptr->GetKey()));
	if (error == Error::kSuccess)
		return {ret, Error::kSuccess};
	return {std::vector<Task>{}, error};
}
Error Schedule::store_tasks(const std::vector<Task> &tasks, bool lock) {
	if (!m_user_ptr->GetInstanceSPtr()->MaintainDirs())
		return Error::kFileIOError;

	std::string encrypted = Encrypt(get_string(tasks), m_user_ptr->GetKey());
	if (lock) {
		std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};

		std::ofstream out{m_file_path};
		if (!out.is_open())
			return Error::kFileIOError;
		out.write(encrypted.data(), (std::streamsize)encrypted.size());
	} else {
		std::ofstream out{m_file_path};
		if (!out.is_open())
			return Error::kFileIOError;
		out.write(encrypted.data(), (std::streamsize)encrypted.size());
	}
	return Error::kSuccess;
}

inline void str_append_uint32(std::string *str, uint32_t n) {
	(*str) += char(n & 0xffu);
	n >>= 8u;
	(*str) += char(n & 0xffu);
	n >>= 8u;
	(*str) += char(n & 0xffu);
	(*str) += char(n >> 8u);
}
std::string Schedule::get_string(const std::vector<Task> &tasks) {
	std::string ret = kStringHeader;
	for (const Task &task : tasks) {
		str_append_uint32(&ret, task.id);
		str_append_uint32(&ret, task.begin_time);
		str_append_uint32(&ret, task.remind_time);
		ret += (char)task.priority;
		ret += (char)task.type;
		ret += task.name;
		ret += '\0';
	}
	return ret;
}

inline uint32_t uint32_from_str(std::string_view str) {
	return uint8_t(str[0]) | (uint8_t(str[1]) << 8u) | (uint8_t(str[2]) << 16u) | (uint8_t(str[3]) << 24u);
}
std::tuple<std::vector<Schedule::Task>, Error> Schedule::parse_string(std::string_view str) {
	if (str.length() < kStringHeaderLength || str.substr(0, kStringHeaderLength) != kStringHeader)
		return {std::vector<Task>{}, Error::kUserWrongPassword};

	std::vector<Task> ret;
	str = str.substr(kStringHeaderLength);

	while (str.length() > 4 + 4 + 4 + 1 + 1) {
		Task task{};
		task.id = uint32_from_str(str);
		str = str.substr(4);
		task.begin_time = uint32_from_str(str);
		str = str.substr(4);
		task.remind_time = uint32_from_str(str);
		str = str.substr(4);
		task.priority = (Priority)str[0];
		task.type = (Type)str[1];
		str = str.substr(2);
		{
			auto num = str.find_first_of('\0');
			if (num == std::string::npos) {
				task.name = str;
				str = str.substr(str.length());
			} else {
				task.name = str.substr(0, num);
				str = str.substr(num + 1); // also remove the zero
			}
		}
		ret.push_back(task);
	}
	return {std::move(ret), Error::kSuccess};
}

void Schedule::operation_thread_func() {
	Operation operation;
	Error error;
	std::vector<Task> tasks;
	while (m_thread_run.load(std::memory_order_acquire)) {
		m_sync_object->operation_queue.wait_dequeue(operation);
		if (operation.op == Operation::kQuit)
			return;
		{
			std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};

			std::tie(tasks, error) = load_tasks(false);
			if (error != Error::kSuccess) {
				operation.error_promise.set_value(error);
				continue;
			}

			printf("Operate\n"); // TODO: Debug
			error = operate(&tasks, operation);
			if (error != Error::kSuccess) {
				operation.error_promise.set_value(error);
				continue;
			}

			error = store_tasks(tasks, false);
			operation.error_promise.set_value(error);
		}
	}
}

void Schedule::sync_thread_func() {
	std::vector<Task> tasks;
	while (m_thread_run.load(std::memory_order_acquire)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (!m_thread_run.load(std::memory_order_acquire))
			return;
		Error error;
		std::tie(tasks, error) = load_tasks(true);
		push_sync_tasks(std::move(tasks));
	}
}

void Schedule::push_sync_tasks(std::vector<Task> &&tasks) const {
	std::scoped_lock tasks_lock{m_sync_tasks_mutex};
	if (tasks != m_sync_tasks) {
		printf("Sync to new version\n"); // TODO: Debug
		m_sync_tasks = std::move(tasks);
		++m_sync_tasks_version;
	}
}

const std::vector<Schedule::Task> &Schedule::GetTasks() const {
	thread_local uint32_t local_tasks_version{0};
	thread_local std::vector<Schedule::Task> local_tasks;
	{
		std::shared_lock tasks_read_lock{m_sync_tasks_mutex};
		if (m_sync_tasks_version > local_tasks_version) {
			printf("Get new version\n"); // TODO: Debug
			local_tasks = m_sync_tasks;
			local_tasks_version = m_sync_tasks_version;
		}
	}
	return local_tasks;
}

} // namespace backend
