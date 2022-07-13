#include <backend/User.hpp>

#include <backend/Schedule.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>

#include <ghc/filesystem.hpp>
#include <libipc/mutex.h>
#include <picosha2.h>
#include <uuid.h>

namespace backend {

struct User::SyncObject {
	static constexpr const char *kIPCMutexHeader = "_SCHEDULITE_MUTEX_";
	ipc::sync::mutex ipc_mutex;
	explicit SyncObject(std::string_view identifier)
	    : ipc_mutex(std::string{kIPCMutexHeader + std::string(identifier)}.c_str()) {}
	~SyncObject() = default;
};

User::User(const std::shared_ptr<Instance> &instance_ptr, std::string_view username, std::string_view password) {
	m_instance_ptr = instance_ptr;
	m_name = username;
	m_file_path =
	    ghc::filesystem::absolute(ghc::filesystem::path{instance_ptr->GetUserDirPath()}.append(m_name)).string();
	// Generate Key
	m_key.resize(picosha2::k_digest_size);
	picosha2::hash256(password, m_key);
	// Generate Identifier
	uuids::uuid_name_generator gen(uuids::uuid::from_string("6bee57f3-7b4a-477b-8b3e-9797ddf842da").value());
	m_identifier = uuids::to_string(gen(m_file_path));
	// Create sync object
	m_sync_object = std::make_unique<SyncObject>(m_identifier);
}

bool User::ValidateUsername(std::string_view username) {
	return username.length() <= kMaxUsernameLength && !username.empty() &&
	       std::all_of(username.begin(), username.end(), isalnum);
}

std::tuple<std::shared_ptr<User>, Error> User::Register(const std::shared_ptr<Instance> &instance_ptr,
                                                        std::string_view username, std::string_view password) {
	if (!ValidateUsername(username))
		return {nullptr, Error::kUserInvalidName};

	std::shared_ptr<User> user = std::make_shared<User>(instance_ptr, username, password);

	if (!instance_ptr->MaintainDirs())
		return {nullptr, Error::kFileIOError};

	{
		std::scoped_lock ipc_lock{user->m_sync_object->ipc_mutex};
		{
			ghc::filesystem::ifstream in{user->m_file_path};
			if (in.is_open())
				return {nullptr, Error::kUserAlreadyExist};
		}
		ghc::filesystem::ofstream out{user->m_file_path};
		if (!out.is_open())
			return {nullptr, Error::kFileIOError};

		out.write(user->m_key.data(), (std::streamsize)user->m_key.size());
	}

	return {std::move(user), Error::kSuccess};
}

std::tuple<std::shared_ptr<User>, Error> User::Login(const std::shared_ptr<Instance> &instance_ptr,
                                                     std::string_view username, std::string_view password) {
	if (!ValidateUsername(username))
		return {nullptr, Error::kUserInvalidName};

	std::shared_ptr<User> user = std::make_shared<User>(instance_ptr, username, password);

	if (!instance_ptr->MaintainDirs())
		return {nullptr, Error::kFileIOError};

	{
		std::scoped_lock ipc_lock{user->m_sync_object->ipc_mutex};
		std::string key;
		{
			ghc::filesystem::ifstream in{user->m_file_path};
			if (!in.is_open())
				return {nullptr, Error::kUserNotFound};
			// get length of file
			in.seekg(0, ghc::filesystem::ifstream::end);
			std::streamsize length = in.tellg();
			in.seekg(0, ghc::filesystem::ifstream::beg);
			// read file
			if (length > 0) {
				key.resize(length);
				in.read((char *)key.data(), length);
			}
		}
		if (key != user->m_key)
			return {nullptr, Error::kUserWrongPassword};
	}

	return {std::move(user), Error::kSuccess};
}

} // namespace backend