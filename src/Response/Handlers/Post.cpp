#include "Response.hpp"

#include <fstream>
#include <sstream>

void Response::_post()
{
	std::string boundaries = _request.get_headers_key("Content-Type");
	size_t boundpos = boundaries.find("boundary=");

	if (boundpos != std::string::npos)
	{
		boundaries = boundaries.substr(boundpos + 9);
		std::istringstream data(this->_request.get_body());
		std::string filename;
		std::string line;
		size_t body_read = 0;
		int token_header = 2;

		while (std::getline(data, line) && token_header)
		{
			if (line.find(boundaries))
				body_read += line.length();
			if (line.find("filename=") != std::string::npos)
			{
				filename = line.substr(line.find("filename=") + 10);
				token_header--;
				body_read += line.length();
			}
			else if (line.find("Content-Type:") != std::string::npos)
			{
				token_header--;
				body_read += line.length();
			}
			if (token_header == 0)
				break;
		}

		filename = filename.substr(0, filename.size() - 2);

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
		data.read(buff, _request.get_content_length() - (5000 * (contentadd - 1) + (body_read + boundaries.length() + 10)));
		file.write(buff, _request.get_content_length() - (5000 * (contentadd - 1) + (body_read + boundaries.length() + 10)));
		file.close();
	}
}
