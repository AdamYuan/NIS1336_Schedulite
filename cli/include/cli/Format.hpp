#ifndef SCHEDULITE_CLI_FORMAT_HPP
#define SCHEDULITE_CLI_FORMAT_HPP

#include <backend/Error.hpp>
#include <backend/Task.hpp>
#include <vector>

namespace cli {

void PrintError(backend::Error error);
void PrintError(std::string_view error_str);
void PrintTasks(const std::vector<backend::Task> &tasks);
template <typename Iter> inline std::string MakeOptionStr(Iter begin, Iter end) {
	std::string ret{*(begin++)};
	for (Iter i = begin; i != end; ++i) {
		ret += '/';
		ret += std::string{*i};
	}
	return ret;
}
template <typename Container> inline std::string MakeOptionStr(Container container) {
	return MakeOptionStr(std::begin(container), std::end(container));
}
} // namespace cli

#endif
