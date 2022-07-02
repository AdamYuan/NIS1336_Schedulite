#include <backend/Environment.hpp>
#include <backend/Instance.hpp>
#include <backend/Schedule.hpp>
#include <backend/Time.hpp>
#include <backend/User.hpp>

#include <cli/Format.hpp>
#include <cli/Shell.hpp>
#include <cli/Util.hpp>

#include <cxxopts.hpp>

static constexpr const char *kExampleShell = " --shell";
static constexpr const char *kExampleUserRegister = " -u USER_NAME -r";
static constexpr const char *kExampleListTasks = " -u USER_NAME -l";
static constexpr const char *kExampleInsertTask =
    " -u USER_NAME -i -t TASK_NAME -b BEGIN_TIME\n      -m REMIND_TIME -p PRIORITY -y TYPE";
static constexpr const char *kExampleEditTask =
    " -u USER_NAME -e TASK_ID [-t NEW_TASK_NAME]\n      [-b NEW_BEGIN_TIME] [-m NEW_REMIND_TIME]\n      [-p "
    "NEW_PRIORITY] [-y NEW_TYPE]";
static constexpr const char *kExampleEraseTasks = " -u USER_NAME -s TASK_ID";
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
	    ("e,edit", "Edit task (with task ID)", cxxopts::value<uint32_t>())                //
	    ("s,erase", "Erase task (with task ID)", cxxopts::value<uint32_t>())              //
	    ("d,done", "Done with a task (toggle, with task ID)", cxxopts::value<uint32_t>()) //
	    ;

	backend::TimeInt time_int_now = backend::GetTimeIntNow();
	std::string time_str_now = backend::ToTimeStr(time_int_now);
	options.add_options("Task")                                    //
	    ("t,taskname", "Task name", cxxopts::value<std::string>()) //
	    ("b,btime", "Begin time (local time, \"YYYY/MM/DD hh:mm\")",
	     cxxopts::value<std::string>()) //
	    ("m,rtime", "Remind time (local time, \"YYYY/MM/DD hh:mm\")",
	     cxxopts::value<std::string>()) //
	    ("p,priority", "Priority (" + cli::MakeOptionStr(backend::GetTaskPriorityStrings()) + ")",
	     cxxopts::value<std::string>()) //
	    ("y,type", "Type (" + cli::MakeOptionStr(backend::GetTaskTypeStrings()) + ")",
	     cxxopts::value<std::string>()) //
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
		std::cout << (                                                      //
		    std::string{"Examples:"} +                                      //
		    "\n  Open Shell: " +                                            //
		    "\n      " + backend::kAppName + kExampleShell +                //
		    "\n\n  Register: " +                                            //
		    "\n      " + backend::kAppName + kExampleUserRegister +         //
		    "\n\n  List Tasks: " +                                          //
		    "\n      " + backend::kAppName + kExampleListTasks +            //
		    "\n\n  Insert Tasks: " +                                        //
		    "\n      " + backend::kAppName + kExampleInsertTask +           //
		    "\n\n  Edit Tasks (ignore args in [] if no need to change): " + //
		    "\n      " + backend::kAppName + kExampleEditTask +             //
		    "\n\n  Erase Tasks: " +                                         //
		    "\n      " + backend::kAppName + kExampleEraseTasks +           //
		    "\n\n  Done Tasks: " +                                          //
		    "\n      " + backend::kAppName + kExampleDoneTasks +            //
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
		printf("App directory: %s\nUser directory: %s\nSchedule directory: %s\nLocal time: %s\n",
		       instance->GetAppDirPath().c_str(), instance->GetUserDirPath().c_str(),
		       instance->GetScheduleDirPath().c_str(), time_str_now.c_str());
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
			std::tie(user, error) = backend::User::Register(instance, username, password);
		} else {
			std::tie(user, error) = backend::User::Login(instance, username, password);
		}

		if (!user) {
			cli::PrintError(error);
			exit(EXIT_FAILURE);
		}
	}

	std::tie(schedule, error) = backend::Schedule::Acquire(user);
	if (!schedule) {
		cli::PrintError(error);
		exit(EXIT_FAILURE);
	}

	if (result.count("list")) {
		cli::PrintTasks(schedule->GetTasks());
		return 0;
	}

	if (result.count("erase")) {
		auto id = result["erase"].as<uint32_t>();
		auto error_future = schedule->TaskErase(id);
		error = error_future.get();
		cli::PrintError(error);
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	if (result.count("done")) {
		auto id = result["done"].as<uint32_t>();
		auto error_future = schedule->TaskToggleDone(id);
		error = error_future.get();
		cli::PrintError(error);
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	if (result.count("insert")) {
		if (!result.count("taskname")) {
			cli::PrintError("Task name not provided");
			exit(EXIT_FAILURE);
		}

		backend::TaskProperty property{};
		property.name = result["taskname"].as<std::string>();
		property.begin_time = property.remind_time = time_int_now;
		if (result.count("btime"))
			property.begin_time = backend::ToTimeInt(result["btime"].as<std::string>());
		if (result.count("rtime"))
			property.remind_time = backend::ToTimeInt(result["rtime"].as<std::string>());
		if (result.count("priority"))
			property.priority = backend::TaskPriorityFromStr(result["priority"].as<std::string>());
		if (result.count("type"))
			property.type = backend::TaskTypeFromStr(result["type"].as<std::string>());

		auto error_future = schedule->TaskInsert(property);
		error = error_future.get();
		cli::PrintError(error);
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	if (result.count("edit")) {
		auto id = result["edit"].as<uint32_t>();

		backend::TaskProperty property{};
		backend::TaskPropertyMask edit_mask{};

		if (result["taskname"].count()) {
			property.name = result["taskname"].as<std::string>();
			edit_mask |= backend::TaskPropertyMask::kName;
		}
		if (result.count("btime")) {
			property.begin_time = backend::ToTimeInt(result["btime"].as<std::string>());
			edit_mask |= backend::TaskPropertyMask::kBeginTime;
		}
		if (result.count("rtime")) {
			property.remind_time = backend::ToTimeInt(result["rtime"].as<std::string>());
			edit_mask |= backend::TaskPropertyMask::kRemindTime;
		}
		if (result.count("priority")) {
			property.priority = backend::TaskPriorityFromStr(result["priority"].as<std::string>());
			edit_mask |= backend::TaskPropertyMask::kPriority;
		}
		if (result.count("type")) {
			property.type = backend::TaskTypeFromStr(result["type"].as<std::string>());
			edit_mask |= backend::TaskPropertyMask::kType;
		}

		auto error_future = schedule->TaskEdit(id, property, edit_mask);
		error = error_future.get();
		cli::PrintError(error);
		exit(error == backend::Error::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	return 0;
}