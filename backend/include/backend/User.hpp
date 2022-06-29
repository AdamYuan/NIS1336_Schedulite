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
class User {
private:
	std::shared_ptr<Instance> m_instance_ptr;
	std::string m_name, m_key;

public:
	explicit User(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password);
	inline ~User() = default;

	static bool ValidateUsername(std::string_view username);

	static std::tuple<std::shared_ptr<User>, std::shared_ptr<Schedule>, Error>
	Register(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password);
	static std::tuple<std::shared_ptr<User>, std::shared_ptr<Schedule>, Error>
	Login(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password);

	inline const std::shared_ptr<Instance> &GetInstanceSPtr() const { return m_instance_ptr; }
	inline const std::string &GetName() const { return m_name; }
	inline const std::string &GetKey() const { return m_key; }
};

} // namespace backend

#endif
