#include <backend/Instance.hpp>

#include <backend/Environment.hpp>
#include <backend/User.hpp>

#include <ghc/filesystem.hpp>
#include <uuid.h>

namespace backend {

Instance::Instance(std::string_view app_dir_path) {
	m_app_dir_path = ghc::filesystem::absolute(ghc::filesystem::path{app_dir_path}).string();
	m_user_dir_path = ghc::filesystem::absolute(ghc::filesystem::path{app_dir_path}.append(kUserDirName)).string();
	m_schedule_dir_path =
	    ghc::filesystem::absolute(ghc::filesystem::path{app_dir_path}.append(kScheduleDirName)).string();
	uuids::uuid_name_generator gen(uuids::uuid::from_string("e7060683-2911-40b1-9d4d-8950c49b1d0e").value());
	m_identifier = uuids::to_string(gen(m_app_dir_path));
}

std::shared_ptr<Instance> Instance::Create() {
	auto ret = std::make_shared<Instance>(GetDefaultAppDirPath());
	if (!ret->MaintainDirs())
		return nullptr;
	return ret;
}
std::shared_ptr<Instance> Instance::Create(std::string_view app_dir_path) {
	auto ret = std::make_shared<Instance>(app_dir_path);
	if (!ret->MaintainDirs())
		return nullptr;
	return ret;
}

std::vector<std::string> Instance::FetchUsernames() const {
	std::vector<std::string> usernames;
	try {
		for (auto const &entry : ghc::filesystem::directory_iterator(m_user_dir_path)) {
			if (entry.is_regular_file()) {
				auto name = entry.path().filename().string();
				if (User::ValidateUsername(name))
					usernames.push_back(name);
			}
		}
	} catch (...) {
		return {};
	}
	return usernames;
}

bool Instance::MaintainDirs() const {
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

		// Maintain schedule dir
		if (ghc::filesystem::exists(m_schedule_dir_path)) {
			if (!ghc::filesystem::is_directory(m_schedule_dir_path))
				return false;
		} else {
			if (!ghc::filesystem::create_directory(m_schedule_dir_path))
				return false;
		}
	} catch (...) {
		return false;
	}
	return true;
}

} // namespace backend