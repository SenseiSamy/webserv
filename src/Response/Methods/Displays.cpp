#include "Response.hpp"

#include <iostream>

void Response::display() const
{
	std::cout << "Response:" << std::endl;
	std::cout << "  status_code: \'" << _status_code << "\'" << std::endl;
	std::cout << "  type: \'";
	switch (_type)
	{
	case HTML:
		std::cout << "HTML";
		break;
	case CSS:
		std::cout << "CSS";
		break;
	case JPG:
		std::cout << "JPG";
		break;
	case ICO:
		std::cout << "ICO";
		break;
	case UNKNOW:
		std::cout << "UNKNOW";
		break;
	}
	std::cout << "\'" << std::endl;
	std::cout << "  status_message: \'" << _status_message << "\'" << std::endl;
	std::cout << "  headers: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << "    \'" << it->first << "\': \'" << it->second << "\' " << std::endl;
	std::cout << "  uri: \'" << _uri << "\'" << std::endl;
	std::cout << "  path_to_root: \'" << _path_to_root << "\'" << std::endl;
	std::cout << "  is_cgi: \'" << _is_cgi << "\'" << std::endl;
}
