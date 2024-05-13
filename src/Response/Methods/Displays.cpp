#include "Response.hpp"

#include <iostream>

void Response::display() const
{
	std::cout << "Response:" << std::endl;
	std::cout << "  status_code: " << _status_code << std::endl;
	std::cout << "  type: " << _type << std::endl;
	std::cout << "  status_message: " << _status_message << std::endl;
	std::cout << "  headers: ";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << " " << it->second << " ";
	std::cout << "  url: " << _url << std::endl;
	std::cout << "  path_to_root: " << _path_to_root << std::endl;
	std::cout << "  is_cgi: " << _is_cgi << std::endl;
}
