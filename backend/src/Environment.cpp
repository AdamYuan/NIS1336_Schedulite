#include <backend/Environment.hpp>

#include <filesystem.hpp>
#include <platform_folders.h>

namespace backend {

std::string GetDefaultAppDirPath() {
	static const std::string ret = ghc::filesystem::path{sago::getDataHome()}.append(kAppDirName);
	return ret;
}

} // namespace backend
