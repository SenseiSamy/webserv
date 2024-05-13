#include "Request.hpp"

#include <iostream>

void Request::display_request() const
{
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << " " << it->second << " ";
}

void Request::display() const
{
	std::cout << "Request:" << std::endl;
	std::cout << "  method: " << _method << std::endl;
	std::cout << "  url: " << _url << std::endl;
	std::cout << "  version: " << _version << std::endl;
	std::cout << "  headers: ";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << " " << it->second << " ";
	std::cout << "  content_length: " << _content_length << std::endl;
}
