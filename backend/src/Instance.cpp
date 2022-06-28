#include <backend/Instance.hpp>

#include <backend/Environment.hpp>

#include <ghc/filesystem.hpp>

namespace backend {

struct InstanceCons : public Instance {
	explicit InstanceCons(std::string_view app_dir_path) {
		m_app_dir_path = ghc::filesystem::path{app_dir_path};
		m_user_dir_path = ghc::filesystem::path{app_dir_path} / kUserDirName;
	}
};

std::shared_ptr<Instance> Instance::Create() {
	std::shared_ptr<Instance> ret = std::make_shared<InstanceCons>(GetDefaultAppDirPath());
	if (!ret->MaintainDirs())
		return nullptr;
	return ret;
}
std::shared_ptr<Instance> Instance::Create(std::string_view app_dir_path) {
	std::shared_ptr<Instance> ret = std::make_shared<InstanceCons>(app_dir_path);
	if (!ret->MaintainDirs())
		return nullptr;
	return ret;
}

bool Instance::MaintainDirs() {
	// Maintain app dir
	try {
		if (ghc::filesystem::exists(m_app_dir_path)) {
			if (!ghc::filesystem::is_directory(m_app_dir_path))
				return false;
		} else {
			if (!ghc::filesystem::create_directory(m_app_dir_path))
				return false;
		}

		// Maintain user dir
		if (ghc::filesystem::exists(m_user_dir_path)) {
			if (!ghc::filesystem::is_directory(m_user_dir_path))
				return false;
		} else {
			if (!ghc::filesystem::create_directory(m_user_dir_path))
				return false;
		}
	} catch (...) {
		return false;
	}
	return true;
}

} // namespace backend