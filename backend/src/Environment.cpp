#include <backend/Environment.hpp>

#include <ghc/filesystem.hpp>
#include <platform_folders.h>

namespace backend {

std::string GetDefaultAppDirPath() {
	static const std::string ret = ghc::filesystem::path{sago::getDataHome()}.append(kAppDirName).string();
	return ret;
}

} // namespace backend
