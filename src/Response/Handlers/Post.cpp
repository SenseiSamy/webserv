#include "Response.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>

void Response::_post()
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
}
