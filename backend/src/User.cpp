#include <backend/User.hpp>

#include <algorithm>
#include <cctype>

#include <picosha2.h>

#include <iostream>

namespace backend {

struct UserCons : public User {
	explicit UserCons(const std::shared_ptr<Instance> &instance_ptr, std::string_view username,
	                  std::string_view password) {
		m_instance_ptr = instance_ptr;
		m_name = username;
		m_key.resize(picosha2::k_digest_size);
		picosha2::hash256(password, m_key);
	}
};

User::~User() {
	if (m_file_io_thread.joinable()) {
		m_file_io_thread_run.store(false, std::memory_order_release);
		m_file_io_semaphore.Signal();
		m_file_io_thread.join();
	}
}

bool User::ValidateUsername(std::string_view username) {
	return std::all_of(username.begin(), username.end(), isalnum);
}

std::tuple<std::shared_ptr<User>, Error> User::Register(const std::shared_ptr<Instance> &instance_ptr,
                                                        std::string_view username, std::string_view password) {
	if (!ValidateUsername(username))
		return {nullptr, Error::kUserInvalidName};

	std::shared_ptr<User> ret = std::make_shared<UserCons>(instance_ptr, username, password);
	Error error;
	std::tie(ret->m_schedule, error) = Schedule::Create(ret, true);

	if (error != Error::kOK)
		return {nullptr, error};
	return {std::move(ret), Error::kOK};
}

std::tuple<std::shared_ptr<User>, Error> User::Login(const std::shared_ptr<Instance> &instance_ptr,
                                                     std::string_view username, std::string_view password) {
	if (!ValidateUsername(username))
		return {nullptr, Error::kUserInvalidName};

	std::shared_ptr<User> ret = std::make_shared<UserCons>(instance_ptr, username, password);
	Error error;
	std::tie(ret->m_schedule, error) = Schedule::Create(ret, false);

	if (error != Error::kOK)
		return {nullptr, error};
	return {std::move(ret), Error::kOK};
}

} // namespace backend