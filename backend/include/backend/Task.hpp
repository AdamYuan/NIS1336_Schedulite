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
 * @brief Task property structure.
 */
struct TaskProperty {
	/** @brief The Task name. */
	std::string name;
	/** @brief The Task's begin time. */
	TimeInt begin_time;
	/** @brief Time to remind. */
	TimeInt remind_time;
	/** @brief The Task priority. */
	TaskPriority priority = kDefaultTaskPriority;
	/** @brief The Task type. */
	TaskType type = kDefaultTaskType;
	/** @brief Whether the Task is done or not. */
	bool done = false;

	inline bool operator==(const TaskProperty &r) const {
		return begin_time == r.begin_time && name == r.name && begin_time == r.begin_time &&
		       remind_time == r.remind_time && priority == r.priority && type == r.type && done == r.done;
	}
	inline bool operator!=(const TaskProperty &r) const { return !operator==(r); }
};

/**
 * @brief Task property mask.
 */
enum class TaskPropertyMask {
	kNone = 0,
	kName = 1 << 0,
	kBeginTime = 1 << 1,
	kKey = kName | kBeginTime,
	kRemindTime = 1 << 2,
	kPriority = 1 << 3,
	kType = 1 << 4,
	kDone = 1 << 5,
	kAll = (1 << 6) - 1
};
inline constexpr TaskPropertyMask operator|(TaskPropertyMask l, TaskPropertyMask r) {
	return (TaskPropertyMask)(int(l) | int(r));
}
inline constexpr TaskPropertyMask operator&(TaskPropertyMask l, TaskPropertyMask r) {
	return (TaskPropertyMask)(int(l) & int(r));
}
inline constexpr TaskPropertyMask operator^(TaskPropertyMask l, TaskPropertyMask r) {
	return (TaskPropertyMask)(int(l) ^ int(r));
}
inline TaskPropertyMask &operator|=(TaskPropertyMask &a, TaskPropertyMask b) {
	return (TaskPropertyMask &)((int &)a |= (int)b);
}
inline TaskPropertyMask &operator&=(TaskPropertyMask &a, TaskPropertyMask b) {
	return (TaskPropertyMask &)((int &)a &= (int)b);
}
inline TaskPropertyMask &operator^=(TaskPropertyMask &a, TaskPropertyMask b) {
	return (TaskPropertyMask &)((int &)a ^= (int)b);
}

/**
 * Patch a TaskProperty object with another TaskProperty and a mask.
 * @brief Patch a TaskProperty object.
 * @param origin The TaskProperty object to be patched.
 * @param patch The TaskProperty patch.
 * @param patch_mask Indicate the element of TaskProperty to be patched.
 * @return The patched TaskProperty.
 */
inline TaskProperty TaskPropertyPatch(const TaskProperty &origin, const TaskProperty &patch,
                                      TaskPropertyMask patch_mask) {
	TaskProperty patched = origin;
	if ((patch_mask & TaskPropertyMask::kName) != TaskPropertyMask::kNone)
		patched.name = patch.name;
	if ((patch_mask & TaskPropertyMask::kBeginTime) != TaskPropertyMask::kNone)
		patched.begin_time = patch.begin_time;
	if ((patch_mask & TaskPropertyMask::kRemindTime) != TaskPropertyMask::kNone)
		patched.remind_time = patch.remind_time;
	if ((patch_mask & TaskPropertyMask::kPriority) != TaskPropertyMask::kNone)
		patched.priority = patch.priority;
	if ((patch_mask & TaskPropertyMask::kType) != TaskPropertyMask::kNone)
		patched.type = patch.type;
	if ((patch_mask & TaskPropertyMask::kDone) != TaskPropertyMask::kNone)
		patched.done = patch.done;
	return patched;
}

/**
 * @brief Task structure.
 */
struct Task {
	/** @brief The Task ID. */
	uint32_t id;
	/** @brief The Task property. */
	TaskProperty property;

	inline bool operator==(const Task &r) const { return id == r.id && property == r.property; }
	inline bool operator!=(const Task &r) const { return id != r.id || property != r.property; }
};

/**
 * Patch a Task object with a TaskProperty and a mask.
 * @brief Patch a Task object.
 * @param origin The Task object to be patched.
 * @param patch The TaskProperty patch.
 * @param patch_mask Indicate the element of TaskProperty to be patched.
 * @return The patched Task.
 */
inline Task TaskPatch(const Task &origin, const TaskProperty &patch, TaskPropertyMask patch_mask) {
	return {origin.id, TaskPropertyPatch(origin.property, patch, patch_mask)};
}

/**
 * Compare less TaskProperty keys (begin_time and name)
 * @brief Compare less TaskProperty keys
 */
inline bool TaskPropertyKeyLess(const TaskProperty &l, const TaskProperty &r) {
	return std::tie(l.begin_time, l.name) < std::tie(r.begin_time, r.name);
}
/**
 * Compare less Task keys (begin_time and name)
 * @brief Compare less Task keys
 */
inline bool TaskKeyLess(const Task &l, const Task &r) { return TaskPropertyKeyLess(l.property, r.property); }
/**
 * Compare equal TaskProperty keys (begin_time and name)
 * @brief Compare equal TaskProperty keys
 */
inline bool TaskPropertyKeyEqual(const TaskProperty &l, const TaskProperty &r) {
	return std::tie(l.begin_time, l.name) == std::tie(r.begin_time, r.name);
}
/**
 * Compare equal Task keys (begin_time and name)
 * @brief Compare equal Task keys
 */
inline bool TaskKeyEqual(const Task &l, const Task &r) { return TaskPropertyKeyEqual(l.property, r.property); }

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
 * Get TaskStatus based on TaskProperty data and current time.
 * @brief Get TaskStatus from TaskProperty.
 */
inline TaskStatus TaskStatusFromTask(const TaskProperty &task_property, TimeInt time_int_now = GetTimeIntNow()) {
	return task_property.done ? TaskStatus::kDone
	                          : (task_property.begin_time > time_int_now ? TaskStatus::kPending : TaskStatus::kBegun);
}

/**
 * Get TaskStatus based on Task data and current time.
 * @brief Get TaskStatus from Task.
 */
inline TaskStatus TaskStatusFromTask(const Task &task, TimeInt time_int_now = GetTimeIntNow()) {
	return TaskStatusFromTask(task.property, time_int_now);
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
