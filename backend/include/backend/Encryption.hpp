#ifndef SCHEDULITE_ENCRYPTION_HPP
#define SCHEDULITE_ENCRYPTION_HPP

#include <string>
#include <string_view>

namespace backend {
/**
 * Encrypt raw string data.
 * @return The encrypted string.
 * @param raw The raw string.
 * @param key The key for encryption.
 */
std::string Encrypt(std::string_view raw, std::string_view key);

/**
 * Decrypt string data.
 * @return The decrypted string.
 * @param encrypted The encrypted string.
 * @param key The key for decryption.
 */
std::string Decrypt(std::string_view encrypted, std::string_view key);
} // namespace backend

#endif
