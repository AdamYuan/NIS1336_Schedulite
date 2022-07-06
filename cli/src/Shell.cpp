#include <cli/Shell.hpp>

#include <cli/Format.hpp>
#include <cli/Util.hpp>

#include <backend/Environment.hpp>
#include <backend/Schedule.hpp>
#include <backend/Task.hpp>

#include <cctype>
#include <iostream>

#include <tinyfiledialogs.h>

#include <random>

namespace cli {

void Shell::Run() {
	printf("%s CLI shell\nApp directory: %s\nUser directory: %s\nSchedule directory: %s\nLocal time: %s\nEnter "
	       "\"help\" for usage hints.\n",
	       backend::kAppName, m_instance_ptr->GetAppDirPath().c_str(), m_instance_ptr->GetUserDirPath().c_str(),
	       m_instance_ptr->GetScheduleDirPath().c_str(), backend::GetTimeStrNow().c_str());
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

Shell::~Shell() { join_reminder_thread(); }

void Shell::print_prompt() { printf("%s > ", m_schedule_ptr ? m_schedule_ptr->GetUserPtr()->GetName().c_str() : ""); }

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
	} else if (cmd == "list" || cmd == "ls") {
		cmd_list();
	} else if (cmd == "insert") {
		cmd_insert();
	} else if (cmd == "edit") {
		cmd_edit();
	} else if (cmd == "erase") {
		cmd_erase();
	} else if (cmd == "test") {
		backend::TaskProperty p{};
		p.name = "Test";
		p.begin_time = std::random_device{}();
		for (uint32_t i = 0; i < 1000; ++i) {
			++p.begin_time;
			PrintError(m_schedule_ptr->TaskInsert(p));
		}
	} else if (cmd == "done") {
		cmd_done();
	} else {
		std::cout << "Unknown command \"" << cmd << "\"" << std::endl;
	}
}

void Shell::cmd_help() {
	printf(R"(help        Show cmd_help text.
login       User login.
register    User register.
list, ls    List all tasks.
insert      Insert a task.
edit        Edit a task.
erase       Erase a task.
done        Done with a task (toggle).
)");
}
void Shell::cmd_login() {
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;

	std::string username = Input("Username");
	std::string password = Input("Password", false);
	std::tie(user, error) = backend::User::Login(m_instance_ptr, username, password);
	if (error != backend::Error::kSuccess) {
		PrintError(error);
		return;
	}
	std::tie(schedule, error) = backend::Schedule::Acquire(user);
	if (error != backend::Error::kSuccess) {
		PrintError(error);
		return;
	}
	join_reminder_thread();
	m_schedule_ptr = std::move(schedule);
	launch_reminder_thread();
}
void Shell::cmd_register() {
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;
	backend::Error error;

	std::string username = Input("Username");
	std::string password = Input("Password", false);
	if (Input("Repeat Password", false) != password) {
		PrintError("Not equal");
		return;
	}
	std::tie(user, error) = backend::User::Register(m_instance_ptr, username, password);
	if (error != backend::Error::kSuccess) {
		PrintError(error);
		return;
	}
	std::tie(schedule, error) = backend::Schedule::Acquire(user);
	if (error != backend::Error::kSuccess) {
		PrintError(error);
		return;
	}
	join_reminder_thread();
	m_schedule_ptr = std::move(schedule);
	launch_reminder_thread();
}
void Shell::cmd_list() {
	if (!m_schedule_ptr) {
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}
	PrintTasks(m_schedule_ptr->GetTasks());
}
void Shell::cmd_insert() {
	if (!m_schedule_ptr) {
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}

	backend::TaskProperty property{};
	property.name = Input("Name");
	property.begin_time = backend::ToTimeInt(Input("Begin time (YYYY/MM/DD hh:mm)"));
	property.remind_time = backend::ToTimeInt(Input("Remind time (YYYY/MM/DD hh:mm)"));
	property.priority = backend::TaskPriorityFromStr(
	    Input((std::string) "Priority (" + MakeOptionStr(backend::GetTaskPriorityStrings()) + ")"));
	property.type =
	    backend::TaskTypeFromStr(Input((std::string) "Type (" + MakeOptionStr(backend::GetTaskTypeStrings()) + ")"));
	PrintError(m_schedule_ptr->TaskInsert(property));
}
void Shell::cmd_edit() {
	if (!m_schedule_ptr) {
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}

	backend::TaskProperty property{};
	backend::TaskPropertyMask edit_mask{};
	uint32_t id;
	{
		try {
			id = std::stoul(Input("ID"));
		} catch (...) {
			PrintError("Bad ID");
			return;
		}
		printf("New properties (leave empty if not change)\n");

		property.name = Input("New name");
		if (!EmptyInput(property.name))
			edit_mask |= backend::TaskPropertyMask::kName;

		std::string input;

		input = Input("New begin time (YYYY/MM/DD hh:mm)");
		if (!EmptyInput(input)) {
			property.begin_time = backend::ToTimeInt(input);
			edit_mask |= backend::TaskPropertyMask::kBeginTime;
		}

		input = Input("New remind time (YYYY/MM/DD hh:mm)");
		if (!EmptyInput(input)) {
			property.remind_time = backend::ToTimeInt(input);
			edit_mask |= backend::TaskPropertyMask::kRemindTime;
		}

		input = Input((std::string) "New priority (" + MakeOptionStr(backend::GetTaskPriorityStrings()) + ")");
		if (!EmptyInput(input)) {
			property.priority = backend::TaskPriorityFromStr(input);
			edit_mask |= backend::TaskPropertyMask::kPriority;
		}

		input = Input((std::string) "New type (" + MakeOptionStr(backend::GetTaskTypeStrings()) + ")");
		if (!EmptyInput(input)) {
			property.type = backend::TaskTypeFromStr(input);
			edit_mask |= backend::TaskPropertyMask::kType;
		}
	}
	PrintError(m_schedule_ptr->TaskEdit(id, property, edit_mask));
}
void Shell::cmd_erase() {
	if (!m_schedule_ptr) {
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}
	uint32_t id;
	try {
		id = std::stoul(Input("ID"));
	} catch (...) {
		PrintError("Bad ID");
		return;
	}

	PrintError(m_schedule_ptr->TaskErase(id));
}
void Shell::cmd_done() {
	if (!m_schedule_ptr) {
		PrintError(backend::Error::kUserNotLoggedIn);
		return;
	}
	uint32_t id;
	try {
		id = std::stoul(Input("ID"));
	} catch (...) {
		PrintError("Bad ID");
		return;
	}

	PrintError(m_schedule_ptr->TaskToggleDone(id));
}

void Shell::launch_reminder_thread() {
	if (m_reminder_thread.joinable())
		return;
	m_thread_run.store(true, std::memory_order_release);
	m_reminder_thread = std::thread(&Shell::reminder_thread_func, this);
}
void Shell::join_reminder_thread() {
	if (m_reminder_thread.joinable()) {
		m_thread_run.store(false, std::memory_order_release);
		m_reminder_cv.notify_all();
		m_reminder_thread.join();
	}
}

void Shell::reminder_thread_func() {
	auto schedule = m_schedule_ptr;
	std::mutex cv_mutex;
	std::unique_lock cv_lock{cv_mutex};

	backend::TimeInt time_int = backend::GetTimeIntNow();

	while (m_thread_run.load(std::memory_order_acquire)) {
		std::string message;
		for (const auto &task : schedule->GetTasks()) {
			if (task.property.remind_time == time_int && !task.property.done) {
				message += backend::ToTimeStr(task.property.begin_time) + " ▶ " + task.property.name;
				if (task.property.type != backend::TaskType::kNone)
					message += (std::string) " (" + backend::StrFromTaskType(task.property.type) + ")";
				message += (std::string) " [" + backend::StrFromTaskPriority(task.property.priority) + " priority]";
				message += '\n';
			}
		}
		if (!message.empty())
			tinyfd_messageBox("Task remind", message.c_str(), "ok", "info", 1);

		m_reminder_cv.wait_until(cv_lock, backend::ToTimePoint(++time_int),
		                         [this]() { return !m_thread_run.load(std::memory_order_acquire); });
	}
}

} // namespace cli
