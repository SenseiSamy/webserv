#include "Response.hpp"

void Response::set_header(const std::string& key, const std::string& value)
{
    _headers[key] = value;
}

void Response::set_body(const std::string& body)
{
    _body = body;
}
