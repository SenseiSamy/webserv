#include "Request.hpp"

void Request::set_headers(const std::map<std::string, std::string> &headers)
{
	_headers = headers;
}

void Request::set_body(const std::string &body)
{
	_body = body;
}
