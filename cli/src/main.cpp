#include <backend/Environment.hpp>
#include <backend/Instance.hpp>
#include <backend/Time.hpp>
#include <backend/User.hpp>

#include <cxxopts.hpp>

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

	options.add_options("Schedule")                 //
	    ("l,list", "List")                          //
	    ("i,insert", "Insert task")                 //
	    ("e,erase", "Erase task (with task index)") //
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

	if (result.count("help")) {
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

	if (result.count("username")) {
		std::string username = result["username"].as<std::string>();

		printf("Password: ");
		std::string password;
		{
			SetStdinEcho(false);
			std::getline(std::cin, password);
			SetStdinEcho(true);
			putchar('\n');
		}

		if (result.count("register")) {
			std::tie(user, error) = backend::User::Register(instance, username, password);
		} else {
			std::tie(user, error) = backend::User::Login(instance, username, password);
		}

		if (!user) {
			printf("%d\n", (int)error);
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}