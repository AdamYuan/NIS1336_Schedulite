#ifndef SCHEDULITE_SCHEDULE_HPP
#define SCHEDULITE_SCHEDULE_HPP

#include <backend/Error.hpp>
#include <backend/Time.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <future>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include <uuid.h>

namespace backend {

class User;
class Schedule {
public:
	inline static constexpr const char *kStringHeader = "Schedule";
	inline static constexpr uint32_t kStringHeaderLength = std::string_view(kStringHeader).length();

	enum class Priority : char { kLow, kMedium, kHigh };
	enum class Type : char { kNone = 0, kStudy, kPlay, kLife };

	struct Task {
		uuids::uuid id;
		std::string name;
		TimeInt begin_time, remind_time;
		Priority priority;
		Type type;
		inline bool KeyLess(const Task &r) const { return std::tie(begin_time, name) < std::tie(r.begin_time, r.name); }
		inline bool KeyEqual(const Task &r) const { return begin_time == r.begin_time && name == r.name; }
	};

protected:
	std::shared_ptr<User> m_user_ptr;
	std::string m_file_path;

	struct SyncObject;
	std::unique_ptr<SyncObject> m_sync_object;

	std::vector<Task> m_local_tasks;

	struct Operation {
		enum Op { kInsert, kErase, kQuit } op{};
		Task task;
		std::promise<Error> error_promise;
	};

	std::atomic_bool m_thread_run{true};
	std::thread m_operation_thread, m_sync_thread;

	void operation_thread_func();

	Error create_file();

	Error store_tasks(const std::vector<Task> &tasks, bool lock);
	std::tuple<std::vector<Task>, Error> load_tasks(bool lock);

	static std::string get_string(const std::vector<Task> &tasks);
	static std::tuple<std::vector<Task>, Error> parse_string(std::string_view str);

	static Error operate(std::vector<Task> *tasks, const Operation &operation);

public:
	static std::tuple<std::unique_ptr<Schedule>, Error> Create(const std::shared_ptr<User> &user_ptr, bool create_file);
	~Schedule();

	inline const std::shared_ptr<User> &GetUserSPTR() const { return m_user_ptr; }
	inline const std::vector<Task> &GetTasks() const { return m_local_tasks; }
	inline const std::string &GetFilePath() const { return m_file_path; }

	std::future<Error> Insert(std::string_view name, TimeInt begin_time, TimeInt remind_time,
	                          Priority priority = Priority::kMedium, Type type = Type::kNone);
	std::future<Error> Erase(const uuids::uuid &id);
};

} // namespace backend

#endif
