#include "Response.hpp"

const int& Response::get_status_code() const
{
    return _status_code;
}

const int& Response::get_type() const
{
    return _type;
}

const ErrorCodes& Response::get_hec() const
{
    return _hec;
}

const std::string& Response::get_status_message() const
{
    return _status_message;
}

const std::string& Response::get_body() const
{
    return _body;
}

const std::string& Response::get_headers_key(const std::string& key) const
{
    return _headers.at(key);
}

const std::map<std::string, std::string>& Response::get_headers() const
{
    return _headers;
}

const Request& Response::get_request() const
{
    return _request;
}

const server_data& Response::get_server() const
{
    return _server;
}
