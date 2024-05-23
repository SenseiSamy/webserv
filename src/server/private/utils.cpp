#include "Server.hpp"

/* Types */
#include <limits.h> // PATH_MAX

inline const std::string Server::_real_path(const std::string &path)
{
	char resolved_path[PATH_MAX];
	if (realpath(path.c_str(), resolved_path) == NULL)
		return "";
	return std::string(resolved_path);
}
