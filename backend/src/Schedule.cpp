#include <backend/Schedule.hpp>

#include <backend/Encryption.hpp>

#include <condition_variable>

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
	m_file_path = ghc::filesystem::path{m_user_ptr->GetInstancePtr()->GetScheduleDirPath()}
	                  .append(m_user_ptr->GetName())
	                  .string();
	m_sync_object = std::make_shared<SyncObject>(m_user_ptr->GetName());
}

std::tuple<std::shared_ptr<Schedule>, Error> Schedule::Create(const std::shared_ptr<User> &user_ptr) {
	std::shared_ptr<Schedule> ret = std::make_shared<Schedule>(user_ptr);
	Error error;

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
	m_sync_thread_cv.notify_all();

	m_sync_object->operation_queue.enqueue({Operation::kQuit});

	if (m_sync_thread.joinable())
		m_sync_thread.join();
	if (m_operation_thread.joinable())
		m_operation_thread.join();
}

std::future<Error> Schedule::TaskInsert(const TaskProperty &task_property) {
	std::promise<Error> promise;
	auto ret = promise.get_future();
	m_sync_object->operation_queue.enqueue({Operation::kInsert, 0, task_property, {}, std::move(promise)});
	return ret;
}

std::future<Error> Schedule::TaskErase(uint32_t id) {
	std::promise<Error> promise;
	auto ret = promise.get_future();
	m_sync_object->operation_queue.enqueue({Operation::kErase, id, {}, {}, std::move(promise)});
	return ret;
}

std::future<Error> Schedule::TaskToggleDone(uint32_t id) {
	std::promise<Error> promise;
	auto ret = promise.get_future();
	m_sync_object->operation_queue.enqueue({Operation::kToggleDone, id, {}, {}, std::move(promise)});
	return ret;
}

std::future<Error> Schedule::TaskEdit(uint32_t id, const TaskProperty &property, TaskPropertyMask property_edit_mask) {
	std::promise<Error> promise;
	auto ret = promise.get_future();
	if (property_edit_mask == TaskPropertyMask::kNone) {
		promise.set_value(Error::kSuccess);
		return ret;
	}
	m_sync_object->operation_queue.enqueue({Operation::kEdit, id, property, property_edit_mask, std::move(promise)});
	return ret;
}

const std::vector<Task> &Schedule::GetTasks() const {
	std::scoped_lock tasks_lock{m_sync_tasks_mutex};

	auto &local_tasks = m_local_tasks[std::this_thread::get_id()];
	if (m_sync_tasks_version > local_tasks.second) {
		printf("Get new version\n");
		local_tasks = {m_sync_tasks, m_sync_tasks_version};
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
Error Schedule::operate(std::vector<Task> *tasks, const Schedule::Operation &operation) {
	if (operation.op == Operation::kInsert) {
		// fetch a unique id
		uint32_t max_id = 0;
		for (auto &task : *tasks)
			max_id = std::max(task.id, max_id);
		return insert(tasks, {max_id + 1, operation.task_property});
	} else {
		// ID based operations
		uint32_t id = operation.id;
		auto it = std::find_if(tasks->begin(), tasks->end(), [&id](const Task &c) { return c.id == id; });
		if (it == tasks->end())
			return Error::kTaskNotFound;

		if (operation.op == Operation::kErase) {
			tasks->erase(it);
		} else if (operation.op == Operation::kToggleDone) {
			it->property.done ^= 1;
		} else if (operation.op == Operation::kEdit) {
			Task task = TaskPatch(*it, operation.task_property, operation.task_property_mask);
			if ((operation.task_property_mask & TaskPropertyMask::kKey) != TaskPropertyMask::kNone) {
				tasks->erase(it);
				return insert(tasks, task);
			} else
				*it = task;
		}
	}
	return Error::kSuccess;
}

std::tuple<std::vector<Task>, Error> Schedule::load_tasks(bool lock) {
	if (!m_user_ptr->GetInstancePtr()->MaintainDirs())
		return {std::vector<Task>{}, Error::kFileIOError};

	std::string encrypted;
	if (lock) {
		std::scoped_lock ipc_lock{m_sync_object->ipc_mutex};
		std::ifstream in{m_file_path};
		if (!in.is_open())
			return {std::vector<Task>{}, Error::kSuccess};
		encrypted = {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	} else {
		std::ifstream in{m_file_path};
		if (!in.is_open())
			return {std::vector<Task>{}, Error::kSuccess};
		encrypted = {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	}

	auto [ret, error] = parse_string(Decrypt(encrypted, m_user_ptr->GetKey()));
	if (error == Error::kSuccess)
		return {ret, Error::kSuccess};
	return {std::vector<Task>{}, error};
}

Error Schedule::store_tasks(const std::vector<Task> &tasks, bool lock) {
	if (!m_user_ptr->GetInstancePtr()->MaintainDirs())
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

std::string Schedule::get_string(const std::vector<Task> &tasks) {
	std::string ret = kStringHeader;
	for (const Task &task : tasks)
		ret += StrFromTask(task);
	return ret;
}
std::tuple<std::vector<Task>, Error> Schedule::parse_string(std::string_view str) {
	if (str.length() < kStringHeaderLength || str.substr(0, kStringHeaderLength) != kStringHeader)
		return {std::vector<Task>{}, Error::kSuccess}; // Return empty if header not match (do not drop error)

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
	std::mutex cv_mutex;
	std::unique_lock cv_lock{cv_mutex};

	std::vector<Task> tasks;
	while (m_thread_run.load(std::memory_order_acquire)) {
		m_sync_thread_cv.wait_for(cv_lock, std::chrono::milliseconds(100));
		if (!m_thread_run.load(std::memory_order_acquire))
			return;
		Error error;
		std::tie(tasks, error) = load_tasks(true);
		if (error == Error::kSuccess)
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

} // namespace backend
