#include "Utils.hpp"
#include "Response.hpp"

#include <dirent.h>
#include <iostream>
#include <linux/limits.h>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool Response::_is_a_directory(const std::string &uri) const
{
	struct stat statbuf;
	if (stat(uri.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISDIR(statbuf.st_mode));
}

bool Response::_is_a_file(const std::string &uri) const
{
	struct stat statbuf;
	if (stat(uri.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISREG(statbuf.st_mode));
}

bool Response::_exists(const std::string &name) const
{
	struct stat buffer;
	if (stat(name.c_str(), &buffer) != 0)
		return (false);
	return (true);
}

bool Response::_write_perm(const std::string &name) const
{
	return (access(name.c_str(), W_OK));
}

int Response::_check_and_rewrite_uri()
{
	if (access((_path_to_root + _uri).c_str(), F_OK) == -1)
		return (404);
	if (access((_path_to_root + _uri).c_str(), R_OK) == -1)
		return (403);
	if (_is_a_directory((_path_to_root + _uri).c_str()))
		return (403);

	char path[PATH_MAX];
	if (realpath((_path_to_root + _uri).c_str(), path) == NULL)
		return (500);
	_uri = path;
	if (getcwd(path, PATH_MAX) == NULL)
		return (500);
	if (_uri.find(_path_to_root) != 0)
		return (403);
	_uri.erase(0, std::string(_path_to_root).size());
	return (0);
}

void Response::_redirect()
{
	if (_route == NULL || _route->redirect.empty()) {
		_generate_response_code(500);
		return;
	}
	set_status_code(307);
	set_headers("Location", _route->redirect);
}

void Response::_directory_listing()
{
	struct dirent *entry;
	DIR *directory = opendir((_path_to_root + _uri).c_str());
	if (directory == NULL)
	{
		_generate_response_code(500);
		return;
	}

	set_status_code(200);
	set_headers("Content-Type", "text/html");

	_body += "<h1>Index of " + _uri + "</h1>\n";
	_body += "<ul>\n";
	_body += "<li><a href=..>..</a></li?>";
	while ((entry = readdir(directory)) != NULL)
	{
		if (std::string(entry->d_name) == ".." || std::string(entry->d_name) == ".")
			continue;
		_body += "<li><a href=\"";
		_body += _uri + "/" + entry->d_name;
		_body += "\">";
		_body += entry->d_name;
		_body += "</a></li>\n";
	}
	_body += "</ul>\n";

	set_content_lenght();
}

void Response::_select_route()
{
	const std::string request = _request.get_uri();

	for (size_t i = 0; i < _server.routes.size(); ++i)
	{
		if (request.find(_server.routes[i].path) == 0)
		{
			for (size_t j = 0; j < _server.routes[i].accepted_methods.size(); ++j)
			{
				if (_request.get_method() == _server.routes[i].accepted_methods[j])
				{
					_route = &_server.routes[i];
					return;
				}
			}
		}
	}
	_route = NULL;
}

const std::string Response::convert() const
{
	std::stringstream ss;
	ss << _status_code;
	std::stringstream dd;
	dd << _body.length();
	std::string response = "HTTP/1.1 ";
	response += ss.str() + " " + _status_message + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		response += it->first + ": " + it->second + "\r\n";

	if (!_is_cgi)
		response += "\r\n";
	response += _body;
	return response;
}

void Response::_find_type()
{
	const std::string filename = _uri;
	if (filename == "/")
		_type = HTML;
	else if (filename.find(".html") != std::string::npos)
		_type = HTML;
	else if (filename.find(".css") != std::string::npos)
		_type = CSS;
	else if (filename.find(".jpg") != std::string::npos)
		_type = JPG;
	else if (filename.find(".ico") != std::string::npos)
		_type = ICO;
	else
		_type = UNKNOW;
}

void Response::_add_content_type()
{
	if (_type == HTML)
		set_headers("Content-Type", "text/html");
	else if (_type == CSS)
		set_headers("Content-Type", "text/css");
	else if (_type == JPG)
		set_headers("Content-Type", "image/jpeg");
	else if (_type == ICO)
		set_headers("Content-Type", "image/vnd.microsoft.icon");
	else
		set_headers("Content-Type", "text/plain");
}

void Response::_set_root()
{
	char tmp[PATH_MAX];
	const char* path;

	if (!_server.root.empty())
		path = _server.root.c_str();
	else
		path = "www";

	if (realpath(path, tmp) != NULL)
		_path_to_root = tmp;
	else
	 	_path_to_root = "";
}

static inline std::string to_string(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

void Response::_generate_response_code(int num)
{
	for (std::map<std::string, std::string>::iterator it = _server.error_pages.begin(); it != _server.error_pages.end();
			 ++it)
	{
		if (std::atoi(it->first.c_str()) == num)
		{
			_uri = "/" + it->second;
			if (access(_uri.c_str(), F_OK) == -1)
				break;
			_get();
			return;
		}
	}

	set_status_code(num);
	set_headers("Content-Type", "text/html");
	_body = "<h1>" + to_string(num) + " " + _error_codes[num] + "</h1>\n";
	set_content_lenght();
}
