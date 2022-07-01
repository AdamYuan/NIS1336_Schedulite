#ifndef SCHEDULITE_CLI_SHELL_HPP
#define SCHEDULITE_CLI_SHELL_HPP

#include <backend/User.hpp>

#include <atomic>
#include <condition_variable>
#include <thread>

namespace cli {

class Shell {
public:
	explicit Shell(std::shared_ptr<backend::Instance> instance_ptr) : m_instance_ptr{std::move(instance_ptr)} {}
	~Shell();
	void Run();

private:
	std::shared_ptr<backend::Instance> m_instance_ptr;
	std::shared_ptr<backend::Schedule> m_schedule_ptr;

	std::atomic_bool m_thread_run{false};
	std::condition_variable m_reminder_cv;
	std::thread m_reminder_thread;

	void launch_reminder_thread();
	void join_reminder_thread();
	void reminder_thread_func();

	static std::string regularize_cmd(std::string_view raw);
	void run_cmd(std::string_view cmd);

	void cmd_help();
	void cmd_register();
	void cmd_login();
	void cmd_list();
	void cmd_insert();
	void cmd_edit();
	void cmd_erase();
	void cmd_done();

	void print_prompt();
};

} // namespace cli

#endif
