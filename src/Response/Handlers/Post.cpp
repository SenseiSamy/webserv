#include "Response.hpp"

#include <sstream>
#include <fstream>

void Response::_post()
{
	std::string boundaries = _request.get_headers_key("Content-Type");
	size_t boundpos = boundaries.find("boundary=");

	if (boundpos != std::string::npos)
	{
		boundaries = boundaries.substr(boundpos + 9);
		std::istringstream data(_request.get_body());
		std::string filename;
		std::string line;
		size_t body_read = 0;
		int token_header = 2;

		while (std::getline(data, line) && token_header)
		{
			body_read += line.length();
			if (line.find("filename=") != std::string::npos)
			{
				filename = line.substr(line.find("filename=") + 10);
				token_header--;
			}
			else if (line.find("Content-Type:") != std::string::npos)
			{
				token_header--;
			}
			if (token_header == 0)
				break;
		}

		filename = filename.substr(0, filename.size() - 2);

		std::getline(data, line);
		std::string file_path = _server.root + "/" + filename;

		std::ofstream file(file_path.c_str(), std::ios::binary);

		while (std::getline(data, line))
		{
			if (line.find(boundaries) != std::string::npos)
				break;
			file << line << std::endl;
		}
		file.close();
	}
}
