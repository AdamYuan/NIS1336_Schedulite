#ifndef SCHEDULITE_INSTANCE_HPP
#define SCHEDULITE_INSTANCE_HPP

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace backend {

/** @brief Application Instance. */
class Instance {
public:
	explicit Instance(std::string_view app_dir_path);
	inline ~Instance() = default;

	/**
	 * Create Instance with default application directory.
	 * @see Create(std::string_view app_dir_path);
	 */
	static std::shared_ptr<Instance> Create();

	/**
	 * Create Instance with a specified working directory.
	 * @see Create();
	 */
	static std::shared_ptr<Instance> Create(std::string_view app_dir_path);

	/**
	 * Check the current working directory and create missing directories.
	 * @brief Maintain the working directory.
	 * @return Successful or not.
	 */
	bool MaintainDirs() const;

	/**
	 * Fetch the available usernames in the current instance.
	 * @brief Fetch available usernames.
	 * @return An array of usernames.
	 */
	std::vector<std::string> FetchUsernames() const;

	/**
	 * Get the APP data directory of the instance.
	 * @return Directory path string.
	 */
	inline const std::string &GetAppDirPath() const { return m_app_dir_path; }

	/**
	 * Get the user data directory of the instance.
	 * @return Directory path string.
	 */
	inline const std::string &GetUserDirPath() const { return m_user_dir_path; }

	/**
	 * Get the schedule data directory of the instance.
	 * @return Directory path string.
	 */
	inline const std::string &GetScheduleDirPath() const { return m_schedule_dir_path; }

	/**
	 * Get an unique identifier of the Instance.
	 * @return Identifier string.
	 */
	inline const std::string &GetIdentifier() const { return m_identifier; }

private:
	std::string m_app_dir_path, m_user_dir_path, m_schedule_dir_path, m_identifier;
};
} // namespace backend

#endif
