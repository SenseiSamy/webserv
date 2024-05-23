#include "Response.hpp"

const Server &Response::get_server() const
{
	return _server;
}

const Request &Response::get_request() const
{
	return _request;
}

const std::string &Response::get_version() const
{
	return _version;
}

const std::string &Response::get_response_str() const
{
	return _response_str;
}

const std::string &Response::get_status_code() const
{
	return _status_code;
}

const std::string &Response::get_status_message() const
{
	return _status_message;
}

const std::map<std::string, std::string> &Response::get_headers() const
{
	return _headers;
}

const std::string &Response::get_body() const
{
	return _body;
}

const route *Response::get_route() const
{
	return _route;
}

const std::string &Response::get_file_path() const
{
	return _file_path;
}

const size_t &Response::get_content_length() const
{
	return _content_length;
}

const std::string &Response::get_content_type() const
{
	return _content_type;
}

const std::ifstream &Response::get_file_stream() const
{
	return _file_stream;
}

const bool &Response::get_is_cgi() const
{
	return _is_cgi;
}

const std::string &Response::get_cgi_script_path() const
{
	return _cgi_script_path;
}

const std::string &Response::get_cgi_output() const
{
	return _cgi_output;
}
