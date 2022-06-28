#ifndef SCHEDULITE_INSTANCE_HPP
#define SCHEDULITE_INSTANCE_HPP

#include <memory>
#include <string>
#include <string_view>

namespace backend {
class Instance {
private:
	std::string m_app_dir_path, m_user_dir_path;

public:
	explicit Instance(std::string_view app_dir_path);
	inline ~Instance() = default;

	static std::shared_ptr<Instance> Create();
	static std::shared_ptr<Instance> Create(std::string_view app_dir_path);

	bool MaintainDirs();

	inline const std::string &GetAppDirPath() const { return m_app_dir_path; }
	inline const std::string &GetUserDirPath() const { return m_user_dir_path; }
};
} // namespace backend

#endif
