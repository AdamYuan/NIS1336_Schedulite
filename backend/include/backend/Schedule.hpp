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
#include <optional>
#include <thread>
#include <unordered_map>

namespace backend {

/** @brief A Schedule token from a User. */
class Schedule {
public:
	explicit Schedule(const std::shared_ptr<User> &user_ptr);
	~Schedule();

	/**
	 * Create Schedule token from a User.
	 * @param user_ptr The Schedule's parent User.
	 * @return Schedule token and Error code.
	 */
	static std::tuple<std::shared_ptr<Schedule>, Error> Create(const std::shared_ptr<User> &user_ptr);

	/**
	 * Get the pointer to the Schedule's parent User.
	 * @brief Get User pointer.
	 */
	inline const std::shared_ptr<User> &GetUserPtr() const { return m_user_ptr; }

	/**
	 * Get all the Tasks in the Schedule.
	 */
	const std::vector<Task> &GetTasks() const;

	/**
	 * Insert a Task asynchronously to the Schedule.
	 * @brief Async insert a Task.
	 * @param task_property The TaskProperty data to be inserted.
	 * @return Async Error code.
	 */
	std::future<Error> TaskInsert(const TaskProperty &task_property);

	/**
	 * Erase a Task asynchronously from the Schedule.
	 * @brief Async erase a Task.
	 * @param id The ID of the Task to be erased.
	 * @return Async Error code.
	 */
	std::future<Error> TaskErase(uint32_t id);

	/**
	 * Edit a Task asynchronously in the Schedule.
	 * @brief Async edit a Task.
	 * @param id The ID of the Task to be edited.
	 * @param property The updated TaskProperty.
	 * @param property_edit_mask Specifying the parts to modify.
	 * @return Async Error code.
	 */
	std::future<Error> TaskEdit(uint32_t id, const TaskProperty &property, TaskPropertyMask property_edit_mask);

	/**
	 * Toggle the Done state of a Task asynchronously in the Schedule.
	 * @brief Async done or undone a Task.
	 * @param id The ID of the Task.
	 * @return Async Error code.
	 */
	std::future<Error> TaskToggleDone(uint32_t id);

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
		enum Op { kInsert, kErase, kToggleDone, kEdit, kQuit } op{};
		uint32_t id;
		TaskProperty task_property;
		TaskPropertyMask task_property_mask;
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

	static Error insert(std::vector<Task> *tasks, const Task &task);
	static Error operate(std::vector<Task> *tasks, const Operation &operation);
};

} // namespace backend

#endif
