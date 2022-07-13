#ifndef SCHEDULITE_USER_HPP
#define SCHEDULITE_USER_HPP

#include <backend/Error.hpp>
#include <backend/Instance.hpp>

#include <atomic>
#include <fstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <vector>

namespace backend {

class Schedule;

/** @brief A User token from a Instance. */
class User {
public:
	/** @brief Maximum username length. */
	static constexpr uint32_t kMaxUsernameLength = 10;

	explicit User(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password);
	inline ~User() = default;

	/**
	 * Validate the given username.
	 * @return Valid or not.
	 */
	static bool ValidateUsername(std::string_view username);

	/**
	 * Register a User.
	 * @param instance_ptr The User's parent Instance.
	 * @param username Username.
	 * @param password Password.
	 * @return User token and Error code.
	 */
	static std::tuple<std::shared_ptr<User>, Error> Register(const std::shared_ptr<Instance> &instance_ptr,
	                                                         std::string_view username, std::string_view password);

	/**
	 * Login.
	 * @param instance_ptr The User's parent Instance.
	 * @param username Username.
	 * @param password Password.
	 * @return User token and Error code.
	 */
	static std::tuple<std::shared_ptr<User>, Error> Login(const std::shared_ptr<Instance> &instance_ptr,
	                                                      std::string_view username, std::string_view password);

	/**
	 * Get the pointer to the User's parent Instance.
	 * @brief Get Instance pointer.
	 */
	inline const std::shared_ptr<Instance> &GetInstancePtr() const { return m_instance_ptr; }

	/**
	 * Get username.
	 * @return Username string.
	 */
	inline const std::string &GetName() const { return m_name; }

	/**
	 * Get user key (encrypted password).
	 * @return User key's byte string.
	 */
	inline const std::string &GetKey() const { return m_key; }

	/**
	 * Get an unique identifier of the User.
	 * @return Identifier string.
	 */
	inline const std::string &GetIdentifier() const { return m_identifier; }

private:
	std::shared_ptr<Instance> m_instance_ptr;
	std::string m_name, m_key, m_file_path, m_identifier;

	struct SyncObject;
	std::shared_ptr<SyncObject> m_sync_object;
};

} // namespace backend

#endif
