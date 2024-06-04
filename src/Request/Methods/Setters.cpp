#include "Request.hpp"

void Request::set_request(const std::string &request)
{
	_request = request;
}

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

void Request::set_headers_key(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void Request::set_content_length(const size_t &content_length)
{
	_content_length = content_length;
}

void Request::set_body(const std::string &body)
{
	_body = body;
}

void Request::set_query_string(const std::string &query_string)
{
	_query_string = query_string;
}

void Request::set_state(const enum state &state)
{
	_state = state;
}

void Request::set_file_size(const std::size_t &file_size)
{
	_file_size = file_size;
}

void Request::set_file_name(const std::string &file_name)
{
	_file_name = file_name;
}
