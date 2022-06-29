#ifndef SCHEDULITE_TASK_HPP
#define SCHEDULITE_TASK_HPP

#include <array>
#include <cinttypes>
#include <string>
#include <tuple>

#include <backend/Time.hpp>

namespace backend {

enum class TaskPriority : char { kLow, kMedium, kHigh };
enum class TaskType : char { kNone = 0, kStudy, kPlay, kLife, kWork };
constexpr TaskPriority kDefaultTaskPriority = TaskPriority::kMedium;
constexpr TaskType kDefaultTaskType = TaskType::kNone;

enum class TaskStatus { kPending, kBegun, kDone };

/**
 * @brief Task structure.
 */
struct Task {
	/** @brief The Task ID. */
	uint32_t id;
	/** @brief The Task name. */
	std::string name;
	/** @brief The Task's begin time. */
	TimeInt begin_time;
	/** @brief Time to remind. */
	TimeInt remind_time;
	/** @brief The Task priority. */
	TaskPriority priority;
	/** @brief The Task type. */
	TaskType type;
	/** @brief Whether the Task is done or not. */
	bool done;

	inline bool operator==(const Task &r) const {
		return begin_time == r.begin_time && name == r.name && begin_time == r.begin_time &&
		       remind_time == r.remind_time && priority == r.priority && type == r.type && done == r.done;
	}
};

/**
 * Compare less Task keys (begin_time and name)
 * @brief Compare less Task keys
 */
inline bool TaskKeyLess(const Task &l, const Task &r) {
	return std::tie(l.begin_time, l.name) < std::tie(r.begin_time, r.name);
}
/**
 * Compare equal Task keys (begin_time and name)
 * @brief Compare equal Task keys
 */
inline bool TaskKeyEqual(const Task &l, const Task &r) {
	return std::tie(l.begin_time, l.name) == std::tie(r.begin_time, r.name);
}

/**
 * Get Task data from an encoded string.
 * @return the Task from string, the deserialized string length (0 if failed)
 * @param str The string to be deserialized.
 */
std::tuple<Task, uint32_t> TaskFromStr(std::string_view str);
/**
 * Serialize Task data to a string.
 * @param task The task to be serialized.
 */
std::string StrFromTask(const Task &task);

/**
 * Get TaskStatus based on task data and current time.
 * @brief Get TaskStatus.
 */
inline TaskStatus TaskStatusFromTask(const Task &task, TimeInt time_int_now = GetTimeIntNow()) {
	return task.done ? TaskStatus::kDone : (task.begin_time > time_int_now ? TaskStatus::kPending : TaskStatus::kBegun);
}

/**
 * Get String descriptor from TaskType enum.
 * @brief Get TaskType's string.
 */
inline constexpr const char *StrFromTaskType(TaskType type) {
	switch (type) {
	case TaskType::kNone:
		return "None";
	case TaskType::kStudy:
		return "Study";
	case TaskType::kPlay:
		return "Play";
	case TaskType::kLife:
		return "Life";
	case TaskType::kWork:
		return "Work";
	default:
		return StrFromTaskType(kDefaultTaskType);
	}
}

/**
 * Get all TaskType strings
 */
inline constexpr std::array<const char *, 5> GetTaskTypeStrings() { return {"None", "Study", "Play", "Life", "Work"}; }

/**
 * Get TaskType from a string, the letter case is ignored.
 * @brief Get TaskType from string.
 */
TaskType TaskTypeFromStr(std::string_view str);

/**
 * Get String descriptor from TaskPriority enum.
 * @brief Get TaskPriority's string.
 */
inline constexpr const char *StrFromTaskPriority(TaskPriority priority) {
	switch (priority) {
	case TaskPriority::kLow:
		return "Low";
	case TaskPriority::kMedium:
		return "Medium";
	case TaskPriority::kHigh:
		return "High";
	default:
		return StrFromTaskPriority(kDefaultTaskPriority);
	}
}

/**
 * Get all TaskPriority strings
 */
inline constexpr std::array<const char *, 3> GetTaskPriorityStrings() { return {"Low", "Medium", "High"}; }

/**
 * Get TaskPriority from a string, the letter case is ignored.
 * @brief Get TaskPriority from string.
 */
TaskPriority TaskPriorityFromStr(std::string_view str);

/**
 * Get String descriptor from TaskStatus enum.
 * @brief Get TaskStatus's string.
 */
inline constexpr const char *StrFromTaskStatus(TaskStatus status) {
	switch (status) {
	case TaskStatus::kPending:
		return "Pending";
	case TaskStatus::kBegun:
		return "Begun";
	case TaskStatus::kDone:
		return "Done";
	default:
		return "Unknown";
	}
}

} // namespace backend

#endif
