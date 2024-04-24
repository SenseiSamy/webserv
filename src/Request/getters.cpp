#include "Request.hpp"

const std::string& Request::get_method() const
{
    return _method;
}

const std::string& Request::get_uri() const
{
    return _uri;
}

const std::string& Request::get_version() const
{
    return _version;
}

const std::map<std::string, std::string>& Request::get_header() const
{
    return _headers;
}

const std::string Request::get_header_value(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    if (it != _headers.end())
        return it->second;
    return "";
}

const std::string& Request::get_body() const
{
    return _body;
}
