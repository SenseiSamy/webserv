#include "Request.hpp"

const std::map<std::string, std::string> &Request::get_headers() const
{
	return _headers;
}

const std::string &Request::get_body() const
{
	return _body;
}
