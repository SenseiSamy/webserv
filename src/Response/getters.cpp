#include "Response.hpp"

const std::string& Response::get_http_version() const
{
    return _http_version;
}

int Response::get_status_code() const
{
    return _status_code;
}

const std::map<std::string, std::string>& Response::get_headers() const
{
    return _headers;
}

const std::string& Response::get_body() const
{
    return _body;
}
