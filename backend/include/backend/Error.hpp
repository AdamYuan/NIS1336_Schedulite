#ifndef SCHEDULITE_ERROR_HPP
#define SCHEDULITE_ERROR_HPP

#include <cinttypes>

namespace backend {

/** @brief Error codes. */
enum class Error : uint8_t {
	kSuccess = 0,

	kFileIOError,

	kUserNotLoggedIn,
	kUserAlreadyExist,
	kUserWrongPassword,
	kUserNotFound,
	kUserInvalidName,

	kTaskNotFound,
	kTaskAlreadyExist,

	kSHMInitializationError,
	kSHMSizeExceed
};

/**
 * Get the corresponding error message from an Error.
 * @brief Get Error message.
 */
inline constexpr const char *GetErrorMessage(Error error) {
	switch (error) {
	case Error::kSuccess:
		return "Success";

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

	case Error::kTaskNotFound:
		return "Task not found";
	case Error::kTaskAlreadyExist:
		return "Task with the same name and begin time already exists";

	case Error::kSHMInitializationError:
		return "Failed to initialize shared memory";
	case Error::kSHMSizeExceed:
		return "Shared memory size exceed";

	default:
		return "(unrecognized error)";
	}
}

} // namespace backend

#endif
