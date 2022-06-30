#ifndef SCHEDULITE_ENVIRONMENT_HPP
#define SCHEDULITE_ENVIRONMENT_HPP

#include <string>

namespace backend {

/** @brief The application name. */
constexpr const char *kAppName = "Schedulite";
/** @brief The application's data directory name. */
constexpr const char *kAppDirName = "schedulite";
/** @brief The application's user data directory name. */
constexpr const char *kUserDirName = "user.d";
/** @brief The application's schedule data directory name. */
constexpr const char *kScheduleDirName = "sched.d";

/**
 * Get the default dir path for storing App data, depending on the operating system settings.
 * @brief Get default dir path for App data.
 */
std::string GetDefaultAppDirPath();

} // namespace backend

#endif
