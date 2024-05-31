#include "Request.hpp"
#include <exception>

const std::string &Request::get_request() const
{
	return _request;
}

const std::string &Request::get_method() const
{
	return _method;
}

const std::string &Request::get_uri() const
{
	return _uri;
}

const std::string &Request::get_version() const
{
	return _version;
}

const std::string Request::get_headers_key(const std::string &key) const
{
	try {
		return _headers.at(key);
	}
	catch (std::exception& e) {
		return ("");
	}
}

const std::map<std::string, std::string> &Request::get_headers() const
{
	return _headers;
}

const size_t &Request::get_content_length() const
{
	return _content_length;
}

const std::string &Request::get_body() const
{
	return _body;
}

const std::string &Request::get_query_string() const
{
	return _query_string;
}

const std::string Request::get_first_line() const
{
	std::string::size_type pos = _request.find("\r\n");
	if (pos == std::string::npos)
		return "";
	return _request.substr(0, pos);
}

enum Request::state Request::get_state() const
{
	return (_state);
}

const std::string Request::get_file_name() const
{
	return (_file_name);
}
