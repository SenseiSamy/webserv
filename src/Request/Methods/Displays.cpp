#include "Request.hpp"

#include <iostream>

void Request::display_request() const
{
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); it++)
        std::cout << it->first << " " << it->second << " ";
}

void Request::display() const
{
    std::cout << "Request:" << std::endl;
    std::cout << "  method: " << _method << std::endl;
    std::cout << "  uri: " << _uri << std::endl;
    std::cout << "  http_version: " << _http_version << std::endl;
    std::cout << "  headers: ";
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); it++)
        std::cout << it->first << " " << it->second << " ";
    std::cout << std::endl;
}
