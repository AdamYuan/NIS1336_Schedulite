#include <cli/Shell.hpp>

#include <cli/Format.hpp>
#include <cli/Util.hpp>

#include <backend/Environment.hpp>
#include <backend/Schedule.hpp>
#include <backend/Task.hpp>

#include <cctype>
#include <iostream>

namespace cli {

void Shell::Run() {
	printf("%s CLI shell\nApp directory: %s\nUser directory: %s\nLocal time: %s\nEnter \"cmd_help\" for usage hints.\n",
	       backend::kAppName, m_instance_ptr->GetAppDirPath().c_str(), m_instance_ptr->GetUserDirPath().c_str(),
	       backend::GetTimeStrNow().c_str());
	std::string cmd;
	print_prompt();
	for (char c = (char)getchar(); c != EOF; c = (char)getchar()) {
		if (c == '\n') {
			cmd = regularize_cmd(cmd);
			if (!cmd.empty())
				run_cmd(cmd);
			cmd.clear();
			print_prompt();
		} else if (c == '\b') {
			// process backspace key
			cmd.pop_back();
		} else {
			// add character
			cmd += c;
		}
	}
}

void Shell::print_prompt() { printf("%s > ", m_schedule_ptr ? m_schedule_ptr->GetUserSPTR()->GetName().c_str() : ""); }

std::string Shell::regularize_cmd(std::string_view raw) {
	std::string cmd;
	for (char c : raw) {
		if (std::isalnum(c))
			cmd.push_back((char)std::tolower(c));
		else
			break;
	}
	return cmd;
}

void Shell::run_cmd(std::string_view cmd) {
	if (cmd == "help") {
		cmd_help();
	} else if (cmd == "login") {
		cmd_login();
	} else if (cmd == "register") {
		cmd_register();
	} else if (cmd == "list") {
		cmd_list();
	} else if (cmd == "insert") {
		cmd_insert();
	} else if (cmd == "erase") {
		cmd_erase();
	} else if (cmd == "done") {
		cmd_done();
	} else {
		std::cout << "Unknown command \"" << cmd << "\"" << std::endl;
	}
}

void Shell::cmd_help() {
	std::scoped_lock lock{m_io_mutex};
	printf(R"(help        Show cmd_help text.
login       User login.
register    User register.
list        List all tasks.
insert      Insert a task.
erase       Erase a task.
done        Done with a task (toggle).
)");
}
void Shell::cmd_login() {
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;

	std::string username, password;
	{
		std::scoped_lock lock{m_io_mutex};
		username = Input("Username");
		password = Input("Password", false);
	}
	std::tie(user, schedule, error) = backend::User::Login(m_instance_ptr, username, password);
	if (error != backend::Error::kSuccess) {
		PrintError(error);
		return;
	}
	// lock schedule
	std::scoped_lock lock{m_schedule_mutex};
	m_schedule_ptr = std::move(schedule);
}
void Shell::cmd_register() {
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;

	std::string username, password;
	{
		std::scoped_lock lock{m_io_mutex};
		username = Input("Username");
		password = Input("Password", false);
		if (Input("Repeat Password", false) != password) {
			PrintError("Not equal");
			return;
		}
	}
	std::tie(user, schedule, error) = backend::User::Register(m_instance_ptr, username, password);
	if (error != backend::Error::kSuccess) {
		PrintError(error);
		return;
	}
	// lock schedule
	std::scoped_lock lock{m_schedule_mutex};
	m_schedule_ptr = std::move(schedule);
}
void Shell::cmd_list() {
	if (!m_schedule_ptr) {
		std::scoped_lock lock{m_io_mutex};
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}
	std::scoped_lock lock{m_io_mutex};
	PrintTasks(m_schedule_ptr->GetTasks());
}
void Shell::cmd_insert() {
	if (!m_schedule_ptr) {
		std::scoped_lock lock{m_io_mutex};
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}

	std::string name;
	backend::TimeInt begin_time, remind_time;
	backend::TaskPriority priority;
	backend::TaskType type;

	{
		std::scoped_lock lock{m_io_mutex};
		name = Input("Name");
		begin_time = backend::ToTimeInt(Input("Begin time"));
		remind_time = backend::ToTimeInt(Input("Remind time"));
		priority = backend::TaskPriorityFromStr(Input("Priority"));
		type = backend::TaskTypeFromStr(Input("Type"));
	}

	PrintError(m_schedule_ptr->Insert(name, begin_time, remind_time, priority, type).get());
}
void Shell::cmd_erase() {
	if (!m_schedule_ptr) {
		std::scoped_lock lock{m_io_mutex};
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}
	uint32_t id;
	std::string id_str;
	{
		std::scoped_lock lock{m_io_mutex};
		id_str = Input("ID");
	}
	try {
		id = std::stoul(id_str);
	} catch (...) {
		PrintError("Bad ID");
		return;
	}

	PrintError(m_schedule_ptr->Erase(id).get());
}
void Shell::cmd_done() {
	if (!m_schedule_ptr) {
		std::scoped_lock lock{m_io_mutex};
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}
	uint32_t id;
	std::string id_str;
	{
		std::scoped_lock lock{m_io_mutex};
		id_str = Input("ID");
	}
	try {
		id = std::stoul(id_str);
	} catch (...) {
		PrintError("Bad ID");
		return;
	}

	PrintError(m_schedule_ptr->ToggleDone(id).get());
}

} // namespace cli
