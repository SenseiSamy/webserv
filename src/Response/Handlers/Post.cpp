#include "Response.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>

static void multipart() 
{

}

void Response::_app_form_urlencoded(std::string	body)
{
	std::map<std::string, std::string> variables;
	std::stringstream ssbody(body);
	std::string	pair;
	while (std::getline(ssbody, pair, '&'))
	{
		std::string field, value;
		size_t	pos = pair.find_first_of('=');
		if (pos == std::string::npos)
			return _generate_error(400);
		field = pair.substr(0, pos);
		value = pair.substr(pos + 1);
		variables[field] = value;
	}
}

void Response::_post()
{
	if (_request.get_headers_key("Content-Type").find("multipart/form-data") != std::string::npos)
		multipart();
	else if (_request.get_headers_key("Content-Type") == "application/x-www-form-urlencoded")
		_app_form_urlencoded(this->_request.get_body());
	else
		_generate_error(400);
}

/*void Response::_post()
{
	std::string boundaries = _request.get_headers_key("Content-Type");
	size_t boundpos = boundaries.find("boundary=");
	if (boundpos != std::string::npos)
	{
		boundaries = boundaries.substr(boundpos + 9);
		// std::cout << boundaries << std::endl;
		std::istringstream data(this->_request.get_body());
		std::string filename;
		std::string line;
		size_t bodyread = 0;
		int tokenheader = 2;
		while (std::getline(data, line) && tokenheader)
		{
			if (line.find(boundaries))
				bodyread += line.length();
			if (line.find("filename=") != std::string::npos)
			{
				filename = line.substr(line.find("filename=") + 10);
				tokenheader--;
				bodyread += line.length();
			}
			else if (line.find("Content-Type:") != std::string::npos)
			{
				tokenheader--;
				bodyread += line.length();
			}
			if (tokenheader == 0)
				break;
		}
		filename = filename.substr(0, filename.size() - 2);

		std::cout << boundaries << std::endl;
		std::getline(data, line);
		std::ofstream file(filename.c_str(), std::ios::binary);
		char buff[5000];
		size_t contentadd = 1;

		while (5000 * contentadd < _request.get_content_length())
		{
			data.read(buff, 5000);
			file.write(buff, 5000);
			contentadd++;
		}
		data.read(buff, _request.get_content_length() -
							(5000 * (contentadd - 1) + (bodyread + boundaries.length() + 10)));
		file.write(buff, _request.get_content_length() -
								(5000 * (contentadd - 1) + (bodyread + boundaries.length() + 10)));
		file.close();

		
	}
}*/
