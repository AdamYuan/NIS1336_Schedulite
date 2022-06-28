#ifndef SCHEDULITE_ENCRYPTION_HPP
#define SCHEDULITE_ENCRYPTION_HPP

#include <string>
#include <string_view>

namespace backend {
std::string Encrypt(std::string_view raw, std::string_view key);
std::string Decrypt(std::string_view encrypted, std::string_view key);
} // namespace backend

#endif
