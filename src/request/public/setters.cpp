#include "Request.hpp"

void Request::set_method(const std::string &method)
{
	_method = method;
}

void Request::set_uri(const std::string &uri)
{
	_uri = uri;
}

void Request::set_version(const std::string &version)
{
	_version = version;
}

void Request::set_headers(const std::map<std::string, std::string> &headers)
{
	_headers = headers;
}
