#include "Response.hpp"

#include <sstream>

void Response::set_status_code(const unsigned short &code)
{
	_status_code = code;
	_status_message = _error_codes[code];
}

void Response::set_body(const std::string &responseBody)
{
	_body = responseBody;
}

void Response::set_headers(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void Response::set_content_lenght()
{
	std::stringstream ss;
	ss << _body.length();
	set_headers("Content-Lenght", ss.str());
}