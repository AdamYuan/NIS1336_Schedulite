#include <backend/User.hpp>

#include <backend/Schedule.hpp>

#include <algorithm>
#include <cctype>

#include <picosha2.h>

#include <iostream>

namespace backend {

User::User(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password) {
	m_instance_ptr = instance_ptr;
	m_name = username;
	m_key.resize(picosha2::k_digest_size);
	picosha2::hash256(password, m_key);
}

bool User::ValidateUsername(std::string_view username) {
	return std::all_of(username.begin(), username.end(), isalnum);
}

std::tuple<std::shared_ptr<User>, std::shared_ptr<Schedule>, Error>
User::Register(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password) {
	if (!ValidateUsername(username))
		return {nullptr, nullptr, Error::kUserInvalidName};

	std::shared_ptr<User> user = std::make_shared<User>(instance_ptr, username, password);
	Error error;
	std::shared_ptr<Schedule> schedule;
	std::tie(schedule, error) = Schedule::Create(user, true);

	if (error != Error::kSuccess)
		return {nullptr, nullptr, error};
	return {std::move(user), std::move(schedule), Error::kSuccess};
}

std::tuple<std::shared_ptr<User>, std::shared_ptr<Schedule>, Error>
User::Login(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password) {
	if (!ValidateUsername(username))
		return {nullptr, nullptr, Error::kUserInvalidName};

	std::shared_ptr<User> user = std::make_shared<User>(instance_ptr, username, password);
	Error error;
	std::shared_ptr<Schedule> schedule;
	std::tie(schedule, error) = Schedule::Create(user, false);

	if (error != Error::kSuccess)
		return {nullptr, nullptr, error};
	return {std::move(user), std::move(schedule), Error::kSuccess};
}

} // namespace backend