#include <backend/Encryption.hpp>

#include <plusaes.hpp>

namespace backend {
constexpr unsigned char kAES_IV[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
std::string Encrypt(std::string_view raw, std::string_view key) {
	std::string encrypted;
	encrypted.resize(plusaes::get_padded_encrypted_size(raw.size()));
	plusaes::encrypt_cbc((unsigned char *)raw.data(), raw.size(), (unsigned char *)key.data(), key.size(), &kAES_IV,
	                     (unsigned char *)encrypted.data(), encrypted.size(), true);
	return encrypted;
}
std::string Decrypt(std::string_view encrypted, std::string_view key) {
	std::string raw;
	raw.resize(encrypted.size());
	unsigned long padded_size = 0;
	plusaes::decrypt_cbc((unsigned char *)encrypted.data(), encrypted.size(), (unsigned char *)key.data(), key.size(),
	                     &kAES_IV, (unsigned char *)raw.data(), raw.size(), &padded_size);
	while (!raw.empty() && padded_size--)
		raw.pop_back();
	return raw;
}
} // namespace backend
