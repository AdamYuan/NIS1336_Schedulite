#include <backend/Environment.hpp>
#include <backend/Instance.hpp>
#include <backend/Schedule.hpp>
#include <backend/Time.hpp>
#include <backend/User.hpp>

#include <cli/Format.hpp>
#include <cli/Shell.hpp>
#include <cli/Util.hpp>

#include <cxxopts.hpp>

static constexpr const char *kExampleUserRegister = " -u USER_NAME -r";
static constexpr const char *kExampleListTasks = " -u USER_NAME -l";
static constexpr const char *kExampleInsertTask =
    " -u USER_NAME -i -t TASK_NAME -b BEGIN_TIME -m REMIND_TIME -p PRIORITY -y TYPE";
static constexpr const char *kExampleEraseTasks = " -u USER_NAME -e TASK_ID";
static constexpr const char *kExampleDoneTasks = " -u USER_NAME -d TASK_ID";

int main(int argc, char **argv) {
	cxxopts::Options options{backend::kAppName, "A simple schedule program"};
	options.add_options()             //
	    ("h,cmd_help", "Print usage") //
	    ("path", "Path for schedule data directory",
	     cxxopts::value<std::string>()->default_value(backend::GetDefaultAppDirPath())) //
	    ("env", "Print program environment")                                            //
	    ("shell", "Run CLI shell")                                                      //
	    ;

	options.add_options("User")                                   //
	    ("u,username", "Username", cxxopts::value<std::string>()) //
	    ("r,register", "Register")                                //
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
	     cxxopts::value<std::string>()->default_value(time_str_now)) //
	    ("p,priority", "Priority (" + cli::MakeOptionStr(backend::GetTaskPriorityStrings()) + ")",
	     cxxopts::value<std::string>()->default_value(backend::StrFromTaskPriority(backend::kDefaultTaskPriority))) //
	    ("y,type", "Type (" + cli::MakeOptionStr(backend::GetTaskTypeStrings()) + ")",
	     cxxopts::value<std::string>()->default_value(backend::StrFromTaskType(backend::kDefaultTaskType))) //
	    ;

	cxxopts::ParseResult result;
	try {
		result = options.parse(argc, argv);
	} catch (cxxopts::OptionException &e) {
		printf("%s\n", e.what());
		exit(EXIT_FAILURE);
	}

	if (result.count("cmd_help") || result.arguments().empty()) {
		printf("%s\n", options.help().c_str());
		std::cout << (                                              //
		    std::string{"Examples:"} +                              //
		    "\n  Register: " +                                      //
		    "\n      " + backend::kAppName + kExampleUserRegister + //
		    "\n  List Tasks: " +                                    //
		    "\n      " + backend::kAppName + kExampleListTasks +    //
		    "\n  Insert Tasks: " +                                  //
		    "\n      " + backend::kAppName + kExampleInsertTask +   //
		    "\n  Erase Tasks: " +                                   //
		    "\n      " + backend::kAppName + kExampleEraseTasks +   //
		    "\n  Done Tasks: " +                                    //
		    "\n      " + backend::kAppName + kExampleDoneTasks +    //
		    "\n\n");
		return 0;
	}

	backend::Error error;

	// Instance creation
	std::shared_ptr<backend::Instance> instance;

	{
		auto path = result["path"].as<std::string>();
		instance = backend::Instance::Create(path);
		if (!instance) {
			printf("ERROR: Invalid data directory path \"%s\"\n", path.c_str());
			exit(EXIT_FAILURE);
		}
	}
	if (result.count("env")) {
		printf("App directory: %s\nUser directory: %s\nLocal time: %s\n", instance->GetAppDirPath().c_str(),
		       instance->GetUserDirPath().c_str(), time_str_now.c_str());
		return 0;
	}

	if (result.count("shell")) {
		cli::Shell shell{instance};
		shell.Run();
		return 0;
	}

	// User
	std::shared_ptr<backend::User> user;
	std::shared_ptr<backend::Schedule> schedule;

	if (result.count("username")) {
		std::string username = result["username"].as<std::string>();

		std::string password = cli::Input("Password", false);

		if (result.count("register")) {
			if (password != cli::Input("Repeat Password", false)) {
				cli::PrintError("Not equal");
				exit(EXIT_FAILURE);
			}
			std::tie(user, schedule, error) = backend::User::Register(instance, username, password);
		} else {
			std::tie(user, schedule, error) = backend::User::Login(instance, username, password);
		}

		if (!user || !schedule) {
			cli::PrintError(error);
			exit(EXIT_FAILURE);
		}
	}

	if (result.count("list")) {
		cli::PrintTasks(schedule->GetTasks());
		return 0;
	}

	if (result.count("erase")) {
		auto id = result["erase"].as<uint32_t>();
		auto error_future = schedule->Erase(id);
		error = error_future.get();
		cli::PrintError(error);
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	if (result.count("done")) {
		auto id = result["done"].as<uint32_t>();
		auto error_future = schedule->ToggleDone(id);
		error = error_future.get();
		cli::PrintError(error);
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	if (result.count("insert")) {
		if (!result.count("taskname")) {
			cli::PrintError("Task name not provided");
			exit(EXIT_FAILURE);
		}
		std::string task_name = result["taskname"].as<std::string>();
		backend::TimeInt task_begin_time = backend::ToTimeInt(result["btime"].as<std::string>());
		backend::TimeInt task_remind_time = backend::ToTimeInt(result["rtime"].as<std::string>());
		backend::TaskPriority task_priority = backend::TaskPriorityFromStr(result["priority"].as<std::string>());
		backend::TaskType task_type = backend::TaskTypeFromStr(result["type"].as<std::string>());

		auto error_future = schedule->Insert(task_name, task_begin_time, task_remind_time, task_priority, task_type);
		error = error_future.get();
		cli::PrintError(error);
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	return 0;
}