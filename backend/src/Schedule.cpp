#include <backend/Schedule.hpp>

#include <backend/Encryption.hpp>
#include <backend/User.hpp>

#include <filesystem.hpp>
#include <libipc/ipc.h>
#include <libipc/mutex.h>
#include <libipc/shm.h>
#include <readerwriterqueue.h>

namespace backend {

struct Schedule::SyncObject {
	static constexpr const char *kIPCMutexHeader = "__SCHEDULITE_SCHEDULE_MUTEX__";
	static constexpr const char *kIPCChannelHeader = "__SCHEDULITE_SCHEDULE_CHANNEL__";
	ipc::sync::mutex ipc_mutex;
	ipc::channel ipc_receiver, ipc_sender;
	ipc::shm::id_t shm_id{};
	moodycamel::BlockingReaderWriterQueue<Operation> operation_queue;
	explicit SyncObject(std::string_view username)
	    : ipc_mutex{std::string{kIPCMutexHeader + std::string(username)}.c_str()},
	      ipc_receiver{std::string{kIPCChannelHeader + std::string(username)}.c_str(), ipc::receiver},
	      ipc_sender{std::string{kIPCChannelHeader + std::string(username)}.c_str(), ipc::sender} {}
	~SyncObject() { ipc::shm::release(shm_id); }
};

struct ScheduleCons : public Schedule {
	explicit ScheduleCons(const std::shared_ptr<User> &user_ptr) {
		m_user_ptr = user_ptr;
		m_file_path = ghc::filesystem::path{m_user_ptr->GetInstanceSPtr()->GetUserDirPath()} / m_user_ptr->GetName();
		m_sync_object = std::make_unique<SyncObject>(m_user_ptr->GetName());
	}
};

std::tuple<std::unique_ptr<Schedule>, Error> Schedule::Create(const std::shared_ptr<User> &user_ptr, bool create_file) {
	std::unique_ptr<Schedule> ret = std::make_unique<ScheduleCons>(user_ptr);
	Error error;
	if (create_file) {
		if ((error = ret->create_file()) != Error::kOK)
			return {nullptr, error};
	}
	std::tie(ret->m_local_tasks, error) = ret->load_tasks(true);
	if (error != Error::kOK)
		return {nullptr, error};
	ret->m_operation_thread = std::thread{&Schedule::operation_thread_func, ret.get()};
	return {std::move(ret), Error::kOK};
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
		auto it = std::lower_bound(tasks->begin(), tasks->end(), operation.task,
		                           [](const Task &l, const Task &r) { return l.KeyLess(r); });
		if (it != tasks->end() && operation.task.KeyEqual(*it))
			return Error::kTaskAlreadyExist;
		tasks->insert(it == tasks->end() ? it : it + 1, operation.task);
	} else {
		// erase
		auto id = operation.task.id;
		auto it = std::find_if(tasks->begin(), tasks->end(), [&id](const Task &c) { return c.id == id; });
		if (it == tasks->end())
			return Error::kTaskNotFound;
		tasks->erase(it);
	}
	return Error::kOK;
}

std::future<Error> Schedule::Insert(std::string_view name, TimeInt begin_time, TimeInt remind_time,
                                    Schedule::Priority priority, Schedule::Type type) {
	std::promise<Error> promise;
	auto ret = promise.get_future();
	m_sync_object->operation_queue.enqueue(
	    {Operation::kInsert,
	     {uuids::uuid_system_generator{}(), std::string{name}, begin_time, remind_time, priority, type},
	     std::move(promise)});
	return ret;
}

std::future<Error> Schedule::Erase(const uuids::uuid &id) {
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
			if (!in.is_open())
				return Error::kUserAlreadyExist;
		}

		std::ofstream out{m_file_path};
		if (!out.is_open())
			return Error::kFileIOError;
		std::string encrypted = Encrypt(kStringHeader, m_user_ptr->GetKey());
		out.write(encrypted.data(), (std::streamsize)encrypted.size());
	}
	return Error::kOK;
}

std::tuple<std::vector<Schedule::Task>, Error> Schedule::load_tasks(bool lock) {
	if (!m_user_ptr->GetInstanceSPtr()->MaintainDirs())
		return {std::vector<Task>{}, Error::kFileIOError};

	std::string encrypted;
	if (lock) {
		std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
		std::ifstream in{m_file_path};
		if (!in.is_open())
			return {std::vector<Task>{}, Error::kFileIOError};
		encrypted = {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	} else {
		std::ifstream in{m_file_path};
		if (!in.is_open())
			return {std::vector<Task>{}, Error::kFileIOError};
		encrypted = {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	}

	auto [ret, error] = parse_string(Decrypt(encrypted, m_user_ptr->GetKey()));
	if (error == Error::kOK)
		return {ret, Error::kOK};
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
	return Error::kOK;
}

inline void str_append_time_int(std::string *str, TimeInt time_int) {
	(*str) += char(time_int & 0xffu);
	time_int >>= 8u;
	(*str) += char(time_int & 0xffu);
	time_int >>= 8u;
	(*str) += char(time_int & 0xffu);
	(*str) += char(time_int >> 8u);
}
std::string Schedule::get_string(const std::vector<Task> &tasks) {
	std::string ret = kStringHeader;
	for (const Task &task : tasks) {
		str_append_time_int(&ret, task.begin_time);
		str_append_time_int(&ret, task.remind_time);
		ret += (char)task.priority;
		ret += (char)task.type;
		ret += task.name;
		ret += '\0';
	}
	return ret;
}

inline TimeInt time_int_from_str(std::string_view str) {
	return uint8_t(str[0]) | (uint8_t(str[1]) << 8u) | (uint8_t(str[2]) << 16u) | (uint8_t(str[3]) << 24u);
}
std::tuple<std::vector<Schedule::Task>, Error> Schedule::parse_string(std::string_view str) {
	if (str.length() < kStringHeaderLength || str.substr(0, kStringHeaderLength) != kStringHeader)
		return {std::vector<Task>{}, Error::kScheduleWrongHeader};

	std::vector<Task> ret;
	str = str.substr(kStringHeaderLength);
	for (size_t begin = str.find_first_not_of('\0'), end = str.find('\0'); begin != std::string_view::npos;
	     begin = str.find_first_not_of('\0', end), end = str.find('\0', begin)) {
		std::string_view part = str.substr(begin, end - begin);

		if (part.length() < 4 + 4 + 1 + 1)
			continue;

		Task task{};
		task.begin_time = time_int_from_str(part);
		task.remind_time = time_int_from_str(part.substr(4, 4));
		task.priority = (Priority)part[8];
		task.type = (Type)part[9];
		task.name = part.substr(4 + 4 + 1 + 1);

		ret.push_back(task);
	}
	return {std::move(ret), Error::kOK};
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
			if (error != Error::kOK) {
				operation.error_promise.set_value(error);
				continue;
			}
			error = operate(&tasks, operation);
			if (error != Error::kOK) {
				operation.error_promise.set_value(error);
				continue;
			}
			error = store_tasks(tasks, false);
			operation.error_promise.set_value(error);
		}
	}
}

} // namespace backend
