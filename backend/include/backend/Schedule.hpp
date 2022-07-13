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

	/**
	 * Acquire Schedule token from a User.
	 * @param user_ptr The Schedule's parent User.
	 * @return Schedule token and Error code.
	 */
	static std::tuple<std::shared_ptr<Schedule>, Error> Acquire(const std::shared_ptr<User> &user_ptr);

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
	 * Get all the Tasks in the Schedule, and check whether the tasks are updated.
	 */
	const std::vector<Task> &GetTasks(bool *p_updated) const;

	/**
	 * Insert a Task to the Schedule.
	 * @brief Insert a Task.
	 * @param task_property The TaskProperty data to be inserted.
	 * @return Inserted task ID and Error code.
	 */
	std::tuple<uint32_t, Error> TaskInsert(const TaskProperty &task_property);

	/**
	 * Erase a Task from the Schedule.
	 * @brief Erase a Task.
	 * @param id The ID of the Task to be erased.
	 * @return Error code.
	 */
	Error TaskErase(uint32_t id);

	/**
	 * Edit a Task in the Schedule.
	 * @brief Edit a Task.
	 * @param id The ID of the Task to be edited.
	 * @param property The updated TaskProperty.
	 * @param property_edit_mask Specifying the parts to modify.
	 * @return Error code.
	 */
	Error TaskEdit(uint32_t id, const TaskProperty &property, TaskPropertyMask property_edit_mask);

	/**
	 * Toggle the Done state of a Task asynchronously in the Schedule.
	 * @brief Done or undone a Task.
	 * @param id The ID of the Task.
	 * @return Error code.
	 */
	Error TaskToggleDone(uint32_t id);

	/**
	 * Get an unique identifier of the Schedule.
	 * @return Identifier string.
	 */
	inline const std::string &GetIdentifier() const { return m_identifier; }

private:
	inline static constexpr const char *kStringHeader = "Schedule";
	inline static constexpr uint32_t kStringHeaderLength = std::string_view(kStringHeader).length();

	std::shared_ptr<User> m_user_ptr;
	std::string m_file_path, m_identifier;

	struct SyncObject;
	std::unique_ptr<SyncObject> m_sync_object;

	// Objects to sync local tasks
	mutable std::mutex m_local_tasks_mutex;
	mutable std::unordered_map<std::thread::id, std::pair<std::vector<Task>, uint32_t>> m_local_tasks;

	Error initialize_shm_locked();
	std::vector<Task> load_tasks_from_shm() const;
	Error store_tasks(const std::vector<Task> &tasks);

	static std::string get_string(const std::vector<Task> &tasks);
	static std::vector<Task> parse_string(std::string_view str);

	static Error insert(std::vector<Task> *tasks, const Task &task);
};

} // namespace backend

#endif
