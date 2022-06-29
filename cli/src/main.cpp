#include <backend/Environment.hpp>
#include <backend/Instance.hpp>
#include <backend/Schedule.hpp>
#include <backend/Time.hpp>
#include <backend/User.hpp>

#include <cxxopts.hpp>
#include <tabulate/table.hpp>

// From https://stackoverflow.com/questions/1413445/reading-a-password-from-stdcin
#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
void SetStdinEcho(bool enable = true) {
#ifdef WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	if (!enable)
		mode &= ~ENABLE_ECHO_INPUT;
	else
		mode |= ENABLE_ECHO_INPUT;
	SetConsoleMode(hStdin, mode);
#else
	termios tty{};
	tcgetattr(STDIN_FILENO, &tty);
	if (!enable)
		tty.c_lflag &= ~ECHO;
	else
		tty.c_lflag |= ECHO;
	(void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__linux__)
#include <sys/ioctl.h>
#endif // Windows/Linux

uint32_t GetTerminalWidth() {
#if defined(_WIN32)
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return (uint32_t)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
#elif defined(__linux__)
	winsize w{};
	ioctl(fileno(stdout), TIOCGWINSZ, &w);
	return (uint32_t)(w.ws_col);
#endif // Windows/Linux
}

std::string EnterPassword() {
	printf("Password: ");
	std::string password;
	SetStdinEcho(false);
	std::getline(std::cin, password);
	SetStdinEcho(true);
	putchar('\n');
	return password;
}

void PrintTasks(const std::vector<backend::Task> &tasks) {
	tabulate::Table table;
	table.add_row({"ID", "Name", "Begin time", "Remind time", "Priority", "Type", "Status"});
	uint32_t row = 1;
	backend::TimeInt time_int_now = backend::GetTimeIntNow();
	for (const auto &task : tasks) {
		auto status = backend::StatusFromTask(task, time_int_now);
		table.add_row({std::to_string(task.id), task.name, backend::ToTimeStr(task.begin_time),
		               backend::ToTimeStr(task.remind_time), backend::StrFromTaskPriority(task.priority),
		               backend::StrFromTaskType(task.type), backend::StrFromTaskStatus(status)});
		if (status == backend::TaskStatus::kBegun)
			table.row(row).format().font_style({tabulate::FontStyle::bold});
		else if (status == backend::TaskStatus::kDone)
			table.row(row).format().font_style({tabulate::FontStyle::dark});
		++row;
	}
	std::cout << table << std::endl;
}

int main(int argc, char **argv) {
	cxxopts::Options options{backend::kAppName, "A simple schedule program"};
	options.add_options()         //
	    ("h,help", "Print usage") //
	    ("path", "Path for schedule data directory",
	     cxxopts::value<std::string>()->default_value(backend::GetDefaultAppDirPath())) //
	    ("env", "Print program environment")                                            //
	    ("shell", "Run CLI shell")                                                      //
	    ;

	options.add_options("User")                                   //
	    ("u,username", "Username", cxxopts::value<std::string>()) //
	    ("r,register", "Register")                                //
	    // ("new_username", "New username", cxxopts::value<std::string>()) //
	    // ("new_password", "New password", cxxopts::value<std::string>()) //
	    ;

	options.add_options("Schedule")                                                       //
	    ("l,list", "List")                                                                //
	    ("i,insert", "Insert task")                                                       //
	    ("e,erase", "Erase task (with task ID)", cxxopts::value<uint32_t>())              //
	    ("d,done", "Done with a task (toggle, with task ID)", cxxopts::value<uint32_t>()) //
	    ;

	std::string time_str_now = backend::GetTimeStrNow();
	options.add_options("Task")                                    //
	    ("t,taskname", "Task name", cxxopts::value<std::string>()) //
	    ("b,btime", "Begin time (local time, \"YYYY/MM/DD hh:mm\")",
	     cxxopts::value<std::string>()->default_value(time_str_now)) //
	    ("m,rtime", "Remind time (local time, \"YYYY/MM/DD hh:mm\")",
	     cxxopts::value<std::string>()->default_value(time_str_now))                                       //
	    ("priority", "Priority (low/medium/high)", cxxopts::value<std::string>()->default_value("medium")) //
	    ("type", "Type (none/study/play/life)", cxxopts::value<std::string>()->default_value("none"))      //
	    ;

	cxxopts::ParseResult result;
	try {
		result = options.parse(argc, argv);
	} catch (cxxopts::OptionException &e) {
		printf("%s\n", e.what());
		exit(EXIT_FAILURE);
	}

	if (result.count("help") || result.arguments().empty()) {
		printf("%s\n", options.help().c_str());
		return 0;
	}

	backend::Error error;

	// Instance creation
	std::shared_ptr<backend::Instance> instance;

	{
		auto path = result["path"].as<std::string>();
		instance = backend::Instance::Create(path);
		if (!instance) {
			printf("Invalid data directory path \"%s\"\n", path.c_str());
			exit(EXIT_FAILURE);
		}
	}
	if (result.count("env")) {
		printf("App directory: %s\nUser directory: %s\nLocal time: %s\n", instance->GetAppDirPath().c_str(),
		       instance->GetUserDirPath().c_str(), time_str_now.c_str());
		return 0;
	}

	if (result.count("shell")) {
		// TODO: Enter shell
		return 0;
	}

	// User
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;

	if (result.count("username")) {
		std::string username = result["username"].as<std::string>();

		std::string password = EnterPassword();

		if (result.count("register")) {
			std::tie(user, schedule, error) = backend::User::Register(instance, username, password);
		} else {
			std::tie(user, schedule, error) = backend::User::Login(instance, username, password);
		}

		if (!user || !schedule) {
			printf("%s\n", backend::GetErrorMessage(error));
			exit(EXIT_FAILURE);
		}
	}

	if (result.count("list")) {
		PrintTasks(schedule->GetTasks());
		return 0;
	}

	if (result.count("erase")) {
		auto id = result["erase"].as<uint32_t>();
		auto error_future = schedule->Erase(id);
		error = error_future.get();
		printf("%s\n", backend::GetErrorMessage(error));
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	if (result.count("done")) {
		auto id = result["done"].as<uint32_t>();
		auto error_future = schedule->ToggleDone(id);
		error = error_future.get();
		printf("%s\n", backend::GetErrorMessage(error));
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	printf("width=%u\n", GetTerminalWidth());

	auto ins = schedule->Insert("Test", backend::GetTimeIntNow(), backend::GetTimeIntNow(),
	                            backend::TaskPriority::kHigh, backend::TaskType::kStudy);
	ins.wait();
	printf("%s\n", backend::GetErrorMessage(ins.get()));

	return 0;
}