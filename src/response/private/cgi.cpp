#include "Response.hpp"

int Response::_is_cgi_script(const std::string &file_path) const
{
	if (_route == NULL || _route->cgi.empty())
		return (0);
	for (std::map<std::string, std::string>::const_iterator cgi = _route->cgi.begin(); cgi != _route->cgi.end(); ++cgi)
	{
		std::string::size_type pos = file_path.find(cgi->first);
		while (pos != std::string::npos)
		{
			if (_get_file_type(_real_path_root + file_path.substr(0, pos + cgi->first.size())) == 0)
				return (1);
			pos = file_path.find(cgi->first, pos + 1);
		}
	}

	return (0);
}