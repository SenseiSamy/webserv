#include "Response.hpp"

void Response::set_status_code(const std::string &status_code)
{
	_status_code = status_code;
}

void Response::set_status_message(const std::string &status_message)
{
	_status_message = status_message;
}

void Response::set_header(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void Response::set_body(const std::string &body)
{
	_body = body;
}

void Response::set_file_path(const std::string &file_path)
{
	_file_path = _real_path(file_path);
}

void Response::set_content_length(const size_t &content_length)
{
	_content_length = content_length;
}

void Response::set_content_type(const std::string &content_type)
{
	_content_type = content_type;
}

void Response::set_is_cgi(const bool &is_cgi)
{
	_is_cgi = is_cgi;
}

void Response::set_cgi_script_path(const std::string &cgi_script_path)
{
	_cgi_script_path = cgi_script_path;
}

void Response::set_cgi_output(const std::string &cgi_output)
{
	_cgi_output = cgi_output;
}
