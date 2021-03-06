#include <backend/Task.hpp>

#include <cctype>

namespace backend {

inline static void str_append_uint32(std::string *str, uint32_t n) {
	(*str) += char(n & 0xffu);
	n >>= 8u;
	(*str) += char(n & 0xffu);
	n >>= 8u;
	(*str) += char(n & 0xffu);
	(*str) += char(n >> 8u);
}

inline static uint32_t uint32_from_str(std::string_view str) {
	return uint8_t(str[0]) | (uint8_t(str[1]) << 8u) | (uint8_t(str[2]) << 16u) | (uint8_t(str[3]) << 24u);
}

std::tuple<Task, uint32_t> TaskFromStr(std::string_view str) {
	if (str.length() <= 4 + 4 + 4 + 1 + 1 + 1)
		return {Task{}, 0};
	Task task{};
	uint32_t len = 4 + 4 + 4 + 1 + 1 + 1;
	task.id = uint32_from_str(str);
	str = str.substr(4);
	task.property.begin_time = uint32_from_str(str);
	str = str.substr(4);
	task.property.remind_time = uint32_from_str(str);
	str = str.substr(4);
	task.property.priority = (TaskPriority)str[0];
	task.property.type = (TaskType)str[1];
	task.property.done = (bool)str[2];
	str = str.substr(3);
	{
		auto num = str.find_first_of('\0');
		if (num == std::string::npos) {
			task.property.name = str;
			len += str.length();
		} else {
			task.property.name = str.substr(0, num);
			len += num + 1;
		}
	}
	return {task, len};
}
std::string StrFromTask(const Task &task) {
	std::string ret;
	str_append_uint32(&ret, task.id);
	str_append_uint32(&ret, task.property.begin_time);
	str_append_uint32(&ret, task.property.remind_time);
	ret += (char)task.property.priority;
	ret += (char)task.property.type;
	ret += (char)task.property.done;
	ret += task.property.name;
	ret += '\0';
	return ret;
}

TaskType TaskTypeFromStr(std::string_view str) {
	if (str.empty())
		return kDefaultTaskType;
	switch (tolower(str[0])) {
	case 'n':
		return TaskType::kNone;
	case 's':
		return TaskType::kStudy;
	case 'p':
		return TaskType::kPlay;
	case 'l':
		return TaskType::kLife;
	case 'w':
		return TaskType::kWork;
	default:
		return kDefaultTaskType;
	}
}

TaskPriority TaskPriorityFromStr(std::string_view str) {
	if (str.empty())
		return kDefaultTaskPriority;
	switch (tolower(str[0])) {
	case 'h':
		return TaskPriority::kHigh;
	case 'm':
		return TaskPriority::kMedium;
	case 'l':
		return TaskPriority::kLow;
	default:
		return kDefaultTaskPriority;
	}
}

TaskStatus TaskStatusFromStr(std::string_view str) {
	if (str.empty())
		return TaskStatus::kPending;
	switch (tolower(str[0])) {
	case 'd':
		return TaskStatus::kDone;
	case 'o':
		return TaskStatus::kOngoing;
	case 'p':
		return TaskStatus::kPending;
	}
	return TaskStatus::kPending;
}

} // namespace backend
