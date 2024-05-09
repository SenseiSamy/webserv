#include "Request.hpp"
#include <exception>

const std::string& Request::get_request() const
{
    return _request;
}

const std::string& Request::get_method() const
{
    return _method;
}

const std::string& Request::get_url() const
{
    return _url;
}

const std::map<std::string, std::string>& Request::get_headers() const
{
    return _headers;
}

const std::string Request::get_headers_key(const std::string& key) const
{
    try {
		return _headers.at(key);
	}
	catch (std::exception &e) {
		return (std::string(""));
	}
}

size_t Request::get_content_lenght() const
{
    return _content_lenght;
}

const std::string& Request::get_body() const
{
    return _body;
}
