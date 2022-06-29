#ifndef SCHEDULITE_CLI_UTIL_HPP
#define SCHEDULITE_CLI_UTIL_HPP

#include <iostream>
#include <string>

namespace cli {

std::string EnterPassword(const char *prompt = "Password: ");
uint32_t GetTerminalWidth();

} // namespace cli

#endif
