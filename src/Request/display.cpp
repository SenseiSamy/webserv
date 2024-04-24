#include "Request.hpp"

#include <iostream>
#include <map>
#include <string>

void Request::display_original_request(int port, const std::string& host) const
{
    std::cout << "\e[38;2;255;148;253m"
              << "╭─ Request to server " << host << ":" << port << "\e[0m" << std::endl;
    for (std::string::size_type i = 0; i < _original_request.size(); ++i)
    {
        if (_original_request[i] == '\r')
            std::cout << std::endl
                      << "\e[38;2;255;148;253m"
                      << "│ "
                      << "\e[0m";
        else
            std::cout << _original_request[i];
    }
    std::cout << "\e[38;2;255;148;253m"
              << "╰───────────────────\e[0m" << std::endl;
}

void Request::display_request(int port, const std::string& host) const
{
    std::cout << "\e[38;2;255;148;253m"
              << "╭─ Request to server " << host << ":" << port << "\e[0m" << std::endl;
    std::cout << "\e[38;2;255;148;253m"
              << "│ " << _method << " " << _uri << " " << _version << "\e[0m" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        std::cout << "\e[38;2;255;148;253m"
                  << "│ " << it->first << ": " << it->second << "\e[0m" << std::endl;
    std::cout << "\e[38;2;255;148;253m"
              << "│ " << _body << "\e[0m" << std::endl;
    std::cout << "\e[38;2;255;148;253m"
              << "╰───────────────────\e[0m" << std::endl;
}