#ifndef SCHEDULITE_CLI_UTIL_HPP
#define SCHEDULITE_CLI_UTIL_HPP

#include <iostream>
#include <string>

namespace cli {

std::string Input(const char *prompt, bool echo = true);
inline std::string Input(const std::string &prompt, bool echo = true) { return Input(prompt.c_str(), echo); }
uint32_t GetTerminalWidth();
bool EmptyInput(std::string_view input);
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
