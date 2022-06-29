#ifndef SCHEDULITE_CLI_SHELL_HPP
#define SCHEDULITE_CLI_SHELL_HPP

#include <backend/User.hpp>

namespace cli {

class Shell {
private:
	std::shared_ptr<backend::Instance> m_instance_ptr;

	std::shared_ptr<backend::User> m_user_ptr;
	std::shared_ptr<backend::Schedule> m_schedule_ptr;

public:
	explicit Shell(std::shared_ptr<backend::Instance> instance_ptr) : m_instance_ptr{std::move(instance_ptr)} {}
	void Run();
};

} // namespace cli

#endif
