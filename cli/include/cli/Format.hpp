#ifndef SCHEDULITE_CLI_FORMAT_HPP
#define SCHEDULITE_CLI_FORMAT_HPP

#include <backend/Error.hpp>
#include <backend/Task.hpp>
#include <vector>

namespace cli {

void PrintError(backend::Error error);
void PrintError(std::string_view error_str);
void PrintTasks(const std::vector<backend::Task> &tasks);

} // namespace cli

#endif
