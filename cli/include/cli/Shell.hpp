#ifndef SCHEDULITE_CLI_SHELL_HPP
#define SCHEDULITE_CLI_SHELL_HPP

#include <backend/User.hpp>
#include <mutex>

namespace cli {

class Shell {
private:
	std::shared_ptr<backend::Instance> m_instance_ptr;
	std::shared_ptr<backend::Schedule> m_schedule_ptr;

	std::mutex m_io_mutex, m_schedule_mutex;

	static std::string regularize_cmd(std::string_view raw);
	void run_cmd(std::string_view cmd);

	void cmd_help();
	void cmd_register();
	void cmd_login();
	void cmd_list();
	void cmd_insert();
	void cmd_erase();
	void cmd_done();

	void print_prompt();

public:
	explicit Shell(std::shared_ptr<backend::Instance> instance_ptr) : m_instance_ptr{std::move(instance_ptr)} {}
	void Run();
};

} // namespace cli

#endif
