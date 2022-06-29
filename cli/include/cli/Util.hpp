#ifndef SCHEDULITE_CLI_UTIL_HPP
#define SCHEDULITE_CLI_UTIL_HPP

#include <iostream>
#include <string>

namespace cli {

std::string Input(const char *prompt, bool echo = true);
uint32_t GetTerminalWidth();

} // namespace cli

#endif
