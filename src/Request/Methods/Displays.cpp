#include "Request.hpp"

#include <iostream>

std::ostream &Request::operator<<(std::ostream &os) const
{
    os << "Request: " << _request << std::endl;
    os << "Method: " << _method << std::endl;
    os << "URI: " << _uri << std::endl;
    os << "Header: " << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it)
        os << "  " << it->first << ": " << it->second << std::endl;
    os << "Content-Length: " << _content_length << std::endl;
    os << "Body: " << _body << std::endl;
    return os;
}
