#include "Response.hpp"

const std::string Response::get_headers_key(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it == _headers.end())
		return ("");
	return (it->second);
}
