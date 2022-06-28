#ifndef SCHEDULITE_ENVIRONMENT_HPP
#define SCHEDULITE_ENVIRONMENT_HPP

#include <cstdlib>
#include <string>

namespace backend {

constexpr const char *kAppName = "Schedulite";

constexpr const char *kAppDirName = "schedulite";
constexpr const char *kUserDirName = "user.d";

std::string GetDefaultAppDirPath();

} // namespace backend

#endif
