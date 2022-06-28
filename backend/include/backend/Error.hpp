#ifndef SCHEDULITE_ERROR_HPP
#define SCHEDULITE_ERROR_HPP

#include <cinttypes>

namespace backend {

enum class Error : uint8_t {
	kOK = 0,

	kFileIOError,

	kUserNotLoggedIn,
	kUserAlreadyExist,
	kUserWrongPassword,
	kUserNotFound,
	kUserInvalidName,
	// kUserLocked,

	kScheduleWrongHeader,
	kScheduleWrongFormat,

	kTaskNotFound,
	kTaskAlreadyExist,
};
inline constexpr const char *GetErrorMessage(Error error) {
	switch (error) {
	case Error::kOK:
		return "No error";

	case Error::kFileIOError:
		return "File IO error";

	case Error::kUserNotLoggedIn:
		return "User not logged in";
	case Error::kUserAlreadyExist:
		return "User already exists";
	case Error::kUserWrongPassword:
		return "Wrong user password";
	case Error::kUserNotFound:
		return "User not found";
	case Error::kUserInvalidName:
		return "Invalid username";

	case Error::kScheduleWrongHeader:
		return "Invalid schedule file header";
	case Error::kScheduleWrongFormat:
		return "Invalid schedule file format";

	case Error::kTaskNotFound:
		return "Task not found";
	case Error::kTaskAlreadyExist:
		return "Task with the same name and begin time already exists";
	default:
		return "(unrecognized error)";
	}
}

} // namespace backend

#endif
