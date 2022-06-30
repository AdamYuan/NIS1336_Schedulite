#ifndef SCHEDULITE_SCHEDULE_HPP
#define SCHEDULITE_SCHEDULE_HPP

#include <backend/Error.hpp>
#include <backend/Task.hpp>
#include <backend/Time.hpp>
#include <backend/User.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace backend {

class Schedule {
public:
	explicit Schedule(const std::shared_ptr<User> &user_ptr);
	static std::tuple<std::shared_ptr<Schedule>, Error> Create(const std::shared_ptr<User> &user_ptr);
	~Schedule();

	inline const std::shared_ptr<User> &GetUserSPTR() const { return m_user_ptr; }
	inline const std::string &GetFilePath() const { return m_file_path; }

	const std::vector<Task> &GetTasks() const;

	std::future<Error> Insert(std::string_view name, TimeInt begin_time, TimeInt remind_time,
	                          TaskPriority priority = TaskPriority::kMedium, TaskType type = TaskType::kNone);
	std::future<Error> Erase(uint32_t id);
	std::future<Error> ToggleDone(uint32_t id);
	// TODO: Operation Edit

private:
	inline static constexpr const char *kStringHeader = "Schedule";
	inline static constexpr uint32_t kStringHeaderLength = std::string_view(kStringHeader).length();

	std::shared_ptr<User> m_user_ptr;
	std::string m_file_path;

	struct SyncObject;
	std::shared_ptr<SyncObject> m_sync_object;

	// Objects to sync local tasks
	mutable std::unordered_map<std::thread::id, std::pair<std::vector<Task>, uint32_t>> m_local_tasks;
	mutable std::mutex m_sync_tasks_mutex;
	mutable uint32_t m_sync_tasks_version{0};
	mutable std::vector<Task> m_sync_tasks;

	struct Operation {
		enum Op { kInsert, kErase, kToggleDone, kQuit } op{};
		Task task;
		std::promise<Error> error_promise;
	};

	std::atomic_bool m_thread_run{true};
	std::condition_variable m_sync_thread_cv;
	std::thread m_operation_thread, m_sync_thread;

	void operation_thread_func();
	void sync_thread_func();

	Error store_tasks(const std::vector<Task> &tasks, bool lock);
	std::tuple<std::vector<Task>, Error> load_tasks(bool lock);

	void push_sync_tasks(std::vector<Task> &&tasks) const;

	static std::string get_string(const std::vector<Task> &tasks);
	static std::tuple<std::vector<Task>, Error> parse_string(std::string_view str);

	static Error operate(std::vector<Task> *tasks, const Operation &operation);
};

} // namespace backend

#endif
