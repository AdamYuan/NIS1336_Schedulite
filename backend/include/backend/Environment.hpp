#ifndef SCHEDULITE_ENVIRONMENT_HPP
#define SCHEDULITE_ENVIRONMENT_HPP

#include <cstdlib>
#include <string>

namespace backend {

constexpr const char *kAppName = "Schedulite";
constexpr const char *kAppDirName = "schedulite";
constexpr const char *kUserDirName = "user.d";

/**
 * Get the default dir path for storing App data, depending on the operating system settings.
 * @brief Get default dir path for App data.
 */
std::string GetDefaultAppDirPath();

} // namespace backend

#endif
