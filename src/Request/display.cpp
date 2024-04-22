#include "Request.hpp"

#include <iostream>

void Request::display_request() const
{
    std::cout << "Method: " << _method << std::endl;
    std::cout << "URL: " << _url << std::endl;
    std::cout << "Headers:\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "Body: " << _body << std::endl;
}

void Request::display_headers() const
{
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
}
