#include "Response.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>
#include <string>
#include <algorithm>

#define BUFFER_SIZE 4096

// getline but removes the \r at the end of a line
static void crlf_getline(std::ifstream& is, std::string& str)
{
	std::getline(is, str);
	if (!str.empty() && str[str.size() - 1] == '\r')
		str.erase(str.size() - 1);
}

static bool multipart_file(std::string filename, std::ifstream& body)
{
	char buffer[BUFFER_SIZE + 1];
	std::size_t	match_counter = 0;

	(void)filename;
	(void)match_counter;
	while (true) {
		body.read(buffer, BUFFER_SIZE);
		buffer[BUFFER_SIZE] = '\0';
		
	}
	return (true);
}

void Response::_multipart() 
{
	std::string line;
	std::size_t boundpos = _request.get_headers_key("Content-Type").find("boundary=");
	if (boundpos == std::string::npos) {
		_generate_response_code(400);
		return;
	}
	std::string boundary = "--" + _request.get_headers_key("Content-Type").substr(boundpos + 9, std::string::npos);

	std::ifstream body(_request.get_file_name().c_str(), std::ios::in
		| std::ios::binary);
	if (!body.is_open()) {
		_generate_response_code(500);
		return;
	}

	std::string filename;
	crlf_getline(body, line);
	while (line != boundary + "--" && body.good()) {
		filename.clear();
		if (line != boundary) {
			_generate_response_code(400);
			return;
		}
		while (!line.empty() && body.good()) {
			crlf_getline(body, line);
			if (line.find("Content-Disposition: form-data;") != std::string::npos) {
				std::size_t pos = line.find("filename=");
				if (pos != std::string::npos) {
					filename = line.substr(pos + 9, std::string::npos);
					filename.erase(std::remove(filename.begin(), filename.end(), '"'), filename.end());
				}
				//else
				// faire qlq chose quand c'est pas un fichier
			}
		}
		if (!body.good()) {
			_generate_response_code(400);
			return;
		}
		if (!filename.empty()) {
			multipart_file(filename, body);
		}
		crlf_getline(body, line);
	}
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
			return _generate_response_code(400);
		field = pair.substr(0, pos);
		value = pair.substr(pos + 1);
		variables[field] = value;
	}
	return _generate_response_code(200);
}

void Response::_post()
{
	if (_request.get_headers_key("Content-Type").find("multipart/form-data") != std::string::npos)
		_multipart();
	else if (_request.get_headers_key("Content-Type") == "application/x-www-form-urlencoded")
		_app_form_urlencoded(this->_request.get_body());
	else
		_generate_response_code(400);
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
