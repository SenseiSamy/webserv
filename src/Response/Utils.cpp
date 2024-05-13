#include "Response.hpp"

#include <dirent.h>
#include <iostream>
#include <linux/limits.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

bool Response::_is_a_directory(const std::string &url) const
{
	struct stat statbuf;
	if (stat(url.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISDIR(statbuf.st_mode));
}

bool Response::_is_a_file(const std::string &url) const
{
	struct stat statbuf;
	if (stat(url.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISREG(statbuf.st_mode));
}

int Response::_check_and_rewrite_url()
{
	if (access((_path_to_root + _url).c_str(), F_OK) == -1)
		return (404);
	if (access((_path_to_root + _url).c_str(), R_OK) == -1)
		return (403);
	if (_is_a_directory((_path_to_root + _url).c_str()))
		return (403);

	char path[PATH_MAX];
	if (realpath((_path_to_root + _url).c_str(), path) == NULL)
		return (500);
	_url = path;
	if (getcwd(path, PATH_MAX) == NULL)
		return (500);
	if (_url.find(_path_to_root) != 0)
		return (403);
	_url.erase(0, std::string(_path_to_root).size());
	return (0);
}


void Response::_redirect()
{
	if (_route == NULL || _route->redirect.empty()) {
		_generate_error(500);
		return;
	}
	set_status_code(307);
	set_status_message(_error_codes[307]);
	set_headers("Location", _route->redirect);
}

void Response::_directory_listing()
{
	struct dirent* entry;
	DIR* directory = opendir((_path_to_root +_url).c_str());
	if (directory == NULL)
	{
		_generate_error(500);
		return;
	}

	set_status_code(200);
	set_status_message(_error_codes[200]);
	set_headers("Content-Type", "text/html");

	_body += "<h1>Index of " + _url + "</h1>\n"; 
	_body += "<ul>\n";
	_body += "<li><a href=..>..</a></li?>";
	while ((entry = readdir(directory)) != NULL)
	{
		if (std::string(entry->d_name) == ".." || std::string(entry->d_name)
			== ".")
			continue;
		_body += "<li><a href=\"";
		_body += _url + "/" + entry->d_name;
		_body += "\">";
		_body += entry->d_name;
		_body += "</a></li>\n";
	}
	_body += "</ul>\n";

	set_content_lenght();
}

void Response::_select_route()
{
	const std::string request = _request.get_url();

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

const std::string Response::to_string() const
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
	std::string filename = _url;
	if (filename == "/")
		this->_type = HTML;
	else if (filename.find(".html") != std::string::npos)
		this->_type = HTML;
	else if (filename.find(".css") != std::string::npos)
		this->_type = CSS;
	else if (filename.find(".jpg") != std::string::npos)
		this->_type = JPG;
	else if (filename.find(".ico") != std::string::npos)
		this->_type = ICO;
	else
		this->_type = UNKNOW;
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
	_path_to_root = realpath("www", tmp);
}

void Response::_generate_error(int num)
{
	for (std::map<std::string, std::string>::iterator it = _server.error_pages.begin(); it != _server.error_pages.end(); ++it)
	{
		if (std::atoi(it->first.c_str()) == num)
		{
			_url = "/" + it->second;
			if (access(_url.c_str(), F_OK) == -1)
				break;
			_get();
			return;
		}
	}

	std::ifstream file("www/ErrorPage");
	std::string line;
	set_status_code(num);
	set_status_message(_error_codes[num]);
	while (std::getline(file, line))
		_body += line + "\n";
	file.close();
	std::srand(time(0));
	std::stringstream ss;
	ss << num;
	int randnum = std::rand();
	if (randnum % 4 == 0)
	{
		_body += "<img src=\"https://http.cat/";
		_body += ss.str() + "\" alt=\"Centered Image\"\n";
		_body += "width=\"800\"\nheight=\"600\"\n/>";
		_body += "</div>\n</body>\n</html>\n\r";
	}
	else if (randnum % 4 == 1)
	{
		_body += "<img src=\"https://http.dog/";
		_body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
		_body += "width=\"800\"\nheight=\"600\"\n/>";
		_body += "</div>\n</body>\n</html>\n\r";
	}

	else if (randnum % 4 == 2)
	{
		_body += "<img src=\"https://http.pizza/";
		_body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
		_body += "width=\"800\"\nheight=\"600\"\n/>";
		_body += "</div>\n</body>\n</html>\n\r";
	}
	else
	{
		_body += "<img src=\"https://httpgoats.com/";
		_body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
		_body += "width=\"800\"\nheight=\"600\"\n/>";
		_body += "</div>\n</body>\n</html>\n\r";
	}
}
