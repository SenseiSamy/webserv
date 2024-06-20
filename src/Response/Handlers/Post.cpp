#include "Response.hpp"

#include <algorithm>
#include <cstddef>
#include <fcntl.h>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>

#define BUFFER_SIZE 4096

// getline but removes the \r at the end of a line
static void crlf_getline(std::ifstream &is, std::string &str)
{
	std::getline(is, str);
	if (!str.empty() && str[str.size() - 1] == '\r')
		str.erase(str.size() - 1);
}

static ssize_t multipart_file(std::string filename, std::ifstream &body, std::string boundary)
{
	char buffer[BUFFER_SIZE + 1];
	std::size_t match_counter = 0;
	std::size_t bytes_read = 0;
	std::ofstream output_file(filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	if (!output_file.is_open())
		return (-1);

	while (true)
	{
		body.read(buffer, BUFFER_SIZE);
		if (body.bad())
			return (-1);
		for (ssize_t i = 0; i < body.gcount(); ++i)
		{
			if (buffer[i] == boundary[match_counter])
			{
				++match_counter;
				if (match_counter == boundary.size())
				{
					output_file.close();
					return (bytes_read);
				}
			}
			else
			{
				if (match_counter > 0)
				{
					output_file.write(boundary.c_str(), match_counter);
					bytes_read += match_counter;
					match_counter = 0;
				}
				output_file.write(&buffer[i], 1);
				++bytes_read;
			}
		}
	}
}

static ssize_t multipart_form(std::ifstream &body, std::string boundary)
{
	char buffer[BUFFER_SIZE + 1];
	std::size_t match_counter = 0;
	std::size_t bytes_read = 0;

	while (true)
	{
		body.read(buffer, BUFFER_SIZE);
		if (body.bad())
			return (-1);
		for (ssize_t i = 0; i < body.gcount(); ++i)
		{
			if (buffer[i] == boundary[match_counter])
			{
				++match_counter;
				if (match_counter == boundary.size())
					return (bytes_read);
			}
			else
			{
				if (match_counter > 0)
				{
					bytes_read += match_counter;
					match_counter = 0;
				}
				++bytes_read;
			}
		}
	}
}

void Response::_multipart()
{
	std::string line;
	std::size_t boundpos = _request.get_headers_key("Content-Type").find("boundary=");
	if (boundpos == std::string::npos)
	{
		_generate_response_code(400); // Bad Request
		return;
	}
	std::string boundary = "--" + _request.get_headers_key("Content-Type").substr(boundpos + 9, std::string::npos);
	std::ifstream body(_request.get_file_name().c_str(), std::ios::in | std::ios::binary);
	if (!body.is_open())
	{
		_generate_response_code(500); // Internal Server Error
		return;
	}

	struct stat st;
	if (stat((_path_to_root + "/upload").c_str(), &st) == -1)
	{
		mkdir((_path_to_root + "/upload").c_str(), 0700);
	}

	std::string filename;
	crlf_getline(body, line);
	while (line != boundary + "--" && !body.bad())
	{
		filename.clear();
		if (line != boundary)
		{
			_generate_response_code(400); // Bad Request
			return;
		}
		while (!line.empty() && body.good())
		{
			crlf_getline(body, line);
			if (line.find("Content-Disposition: form-data;") != std::string::npos)
			{
				std::size_t pos = line.find("filename=");
				if (pos != std::string::npos)
				{
					filename = line.substr(pos + 9, std::string::npos);
					filename.erase(std::remove(filename.begin(), filename.end(), '"'), filename.end());
				}
			}
		}
		if (!body.good())
		{
			_generate_response_code(400); // Bad Request
			return;
		}
		std::streampos begin = body.tellg();
		ssize_t i;
		if (!filename.empty() && _is_a_directory((_path_to_root + _uri).c_str()) && _route && std::find(_route->accepted_methods.begin(), _route->accepted_methods.end(), "POST") != _route->accepted_methods.end())
			i = multipart_file(_path_to_root + _uri + "/" + filename, body, boundary);
		else if (!filename.empty())
			i = multipart_file(_path_to_root + "/upload/" + filename, body, boundary);
		else
			i = multipart_form(body, boundary);
		if (i == -1)
		{
			_generate_response_code(400); // Bad Request
			return;
		}
		body.clear();
		body.seekg(begin);
		body.seekg(i, std::ios_base::cur);
		crlf_getline(body, line);
	}
	_generate_response_code(201); // Created
}

void Response::_app_form_urlencoded()
{
	int fd = open(_request.get_file_name().c_str(), O_RDONLY);
	if (fd == -1)
		return;
	int status_code = _cgi(fd);
	if (status_code == 1)
		return;
	if (status_code > 1)
	{
		_is_cgi = false;
		_generate_response_code(status_code);
		return;
	}
}

void Response::_post()
{
	if (get_request().get_uri() == "/")
		_uri = "/index.html";
	else if (_uri.empty())
		_uri = _request.get_uri();

	if (_request.get_headers_key("Content-Type").find("multipart/form-data") != std::string::npos)
		_multipart();
	else if (_request.get_headers_key("Content-Type") == "application/x-www-form-urlencoded")
		_app_form_urlencoded();
	else
		_generate_response_code(400); // Bad Request
}
