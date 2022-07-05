#ifndef SCHEDULITE_GTK_ICON_HPP
#define SCHEDULITE_GTK_ICON_HPP

#include <backend/Task.hpp>

namespace gui {
inline constexpr const char *GetTaskStatusIconName(backend::TaskStatus status) {
	switch (status) {
	case backend::TaskStatus::kPending:
		return "user-status-pending";
	case backend::TaskStatus::kBegun:
		return "emblem-urgent";
	case backend::TaskStatus::kDone:
		return "object-select";
	}
	return "user-status-pending";
}

inline constexpr const char *GetTaskPriorityIconName(backend::TaskPriority priority) {
	switch (priority) {
	case backend::TaskPriority::kHigh:
		return "starred";
	case backend::TaskPriority::kMedium:
		return "semi-starred";
	case backend::TaskPriority::kLow:
		return "non-starred";
	}
	return GetTaskPriorityIconName(backend::kDefaultTaskPriority);
}

inline constexpr const char *GetTaskTypeIconName(backend::TaskType type) {
	switch (type) {
	case backend::TaskType::kNone:
		return "task-due";
	case backend::TaskType::kWork:
		return "applications-office";
	case backend::TaskType::kStudy:
		return "accessories-dictionary";
	case backend::TaskType::kLife:
		return "preferences-desktop-accessibility";
	case backend::TaskType::kPlay:
		return "applications-games";
	}
	return GetTaskPriorityIconName(backend::kDefaultTaskPriority);
}
}

#endif
