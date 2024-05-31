#include "Response.hpp"

#include <sstream>

void Response::set_status_code(int code)
{
	_status_code = code;
}

void Response::set_status_message(const std::string& message)
{
	_status_message = message;
}

void Response::setBody(const std::string& responseBody)
{
	_body = responseBody;
}

void Response::set_headers(const std::string& key, const std::string& value)
{
	_headers[key] = value;
}

void Response::set_content_lenght()
{
	std::stringstream ss;
	ss << _body.length();
	set_headers("Content-Lenght", ss.str());
}