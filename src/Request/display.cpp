#include "Request.hpp"

#include <iostream>
#include <map>
#include <string>

void Request::display_original_request() const
{
    std::cout << _original_request << std::endl;
}

void Request::display() const
{
    std::cout << "Method: " << _method << std::endl;
    std::cout << "URI: " << _uri << std::endl;
    std::cout << "Version: " << _version << std::endl;
    std::cout << "Headers: " << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Body: " << _body << std::endl;
}
