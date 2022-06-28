#ifndef SCHEDULITE_USER_HPP
#define SCHEDULITE_USER_HPP

#include <backend/Error.hpp>
#include <backend/Instance.hpp>
#include <backend/Schedule.hpp>
#include <backend/Semaphore.hpp>

#include <atomic>
#include <fstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <vector>

namespace backend {

class User {
protected:
	std::shared_ptr<Instance> m_instance_ptr;

	std::string m_name, m_key;

	std::atomic_bool m_file_io_thread_run{true};
	std::thread m_file_io_thread;
	BinarySemaphore m_file_io_semaphore;

	std::unique_ptr<Schedule> m_schedule;

	void file_io_thread_func();

public:
	~User();

	static bool ValidateUsername(std::string_view username);

	static std::tuple<std::shared_ptr<User>, Error> Register(const std::shared_ptr<Instance> &instance_ptr,
	                                                         std::string_view username, std::string_view password);
	static std::tuple<std::shared_ptr<User>, Error> Login(const std::shared_ptr<Instance> &instance_ptr,
	                                                      std::string_view username, std::string_view password);

	inline const std::shared_ptr<Instance> &GetInstanceSPtr() const { return m_instance_ptr; }
	inline const std::unique_ptr<Schedule> &GetScheduleUPtr() const { return m_schedule; }
	inline const std::string &GetName() const { return m_name; }
	inline const std::string &GetKey() const { return m_key; }
};

} // namespace backend

#endif
