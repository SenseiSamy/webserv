#include "Response.hpp"

#include <iostream>

void Response::display() const
{
    std::cout << "Response:" << std::endl;
    std::cout << "  status_code: " << _status_code << std::endl;
    std::cout << "  type: " << _type << std::endl;
    std::cout << "  status_message: " << _status_message << std::endl;
    std::cout << "  body: " << _body << std::endl;
    std::cout << "  header: ";
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); it++)
        std::cout << it->first << " " << it->second << " ";
    std::cout << std::endl;
}

