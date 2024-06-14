#include "Response.hpp"

#include <algorithm>
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
		return (404); // Not Found
	if (access((_path_to_root + _uri).c_str(), R_OK) == -1)
		return (403); // Forbidden
	if (_is_a_directory((_path_to_root + _uri).c_str()))
		return (403); // Forbidden

	char path[PATH_MAX];
	if (realpath((_path_to_root + _uri).c_str(), path) == NULL)
		return (500); // Internal Server Error
	_uri = path;
	if (getcwd(path, PATH_MAX) == NULL)
		return (500); // Internal Server Error
	if (_uri.find(_path_to_root) != 0)
		return (403); // Forbidden
	_uri.erase(0, std::string(_path_to_root).size());
	return (0);
}

void Response::_redirect()
{
	if (_route == NULL || _route->redirect.empty())
	{
		_generate_response_code(500);
		return;
	}
	set_status_code(307);
	set_status_message(_error_codes[307]);
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
	set_status_message(_error_codes[200]);
	set_headers("Content-Type", "text/html");

	_body += "<!DOCTYPE html>\n<html lang=\"en\">\n"
					 "<head>\n\t<meta charset=\"UTF-8\">\n"
					 "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
					 "\t<title>Index of " +
					 _uri +
					 "</title>\n"
					 "\t<h1>Index of " +
					 _uri +
					 "</h1>\n"
					 "\t<script>\n"
					 "\t\tasync function deleteFile(fileName) {\n"
					 "\t\t\tif (confirm(`Are you sure you want to delete ${fileName}?`)) {\n"
					 "\t\t\t\ttry {\n"
					 "\t\t\t\t\tconst response = await fetch(fileName, {\n"
					 "\t\t\t\t\t\tmethod: 'DELETE'\n"
					 "\t\t\t\t\t});\n"
					 "\t\t\t\t\tif (response.ok) {\n"
					 "\t\t\t\t\t\talert(`File ${fileName} deleted successfully.`);\n"
					 "\t\t\t\t\t\tlocation.reload();\n"
					 "\t\t\t\t\t} else {\n"
					 "\t\t\t\t\t\talert(`Failed to delete ${fileName}.`);\n"
					 "\t\t\t\t\t}\n"
					 "\t\t\t\t} catch (error) {\n"
					 "\t\t\t\t\tconsole.error('Error:', error);\n"
					 "\t\t\t\t\talert(`Error deleting ${fileName}.`);\n"
					 "\t\t\t\t}\n\t\t\t}\n\t\t}\n"
					 "\n\t\tasync function uploadFile(event) {\n"
					 "\t\t\tevent.preventDefault();\n"
					 "\t\t\tconst fileInput = document.getElementById('fileInput');\n"
					 "\t\t\tconst file = fileInput.files[0];\n"
					 "\t\t\tif (!file) {\n"
					 "\t\t\t\talert('Please select a file to upload.');\n"
					 "\t\t\t\treturn;\n\t\t\t}\n"
					 "\t\t\tconst formData = new FormData();\n"
					 "\t\t\tformData.append('file', file);\n\n"
					 "\t\t\ttry {\n"
					 "\t\t\t\tconst response = await fetch('" +
					 _uri +
					 "', {\n"
					 "\t\t\t\t\tmethod: 'POST',\n"
					 "\t\t\t\t\tbody: formData\n"
					 "\t\t\t\t});\n"
					 "\t\t\t\tif (response.ok) {\n"
					 "\t\t\t\t\talert('File uploaded successfully.');\n"
					 "\t\t\t\t\tlocation.reload();\n"
					 "\t\t\t\t} else {\n"
					 "\t\t\t\t\talert('Failed to upload file.');\n\t\t\t\t}\n"
					 "\t\t\t} catch (error) {\n"
					 "\t\t\t\tconsole.error('Error:', error);\n"
					 "\t\t\t\talert('Error uploading file.');\n"
					 "\t\t\t}\n\t\t}\n"
					 "\t</script>\n"
					 "</head>\n"
					 "<body>\n"
					 "\t<ul>\n"
					 "\t\t<li><a href=..>..</a></li>\n";
	while ((entry = readdir(directory)) != NULL)
	{
		if (std::string(entry->d_name) == ".." || std::string(entry->d_name) == ".")
			continue;
		_body += "\t\t<li class=\"file-name\">\n\t\t<a href=\"";
		_body += _uri;
		if (_uri[_uri.size() - 1] != '/')
			_body += "/";
		_body += entry->d_name;
		_body += "\t\">";
		_body += entry->d_name;
		_body += "\t</a>\n";
		if (std::find(_route->accepted_methods.begin(), _route->accepted_methods.end(), "DELETE") !=
				_route->accepted_methods.end())
		{
			_body += "\t\t\t<button class=\"delete-btn\" onclick=\"deleteFile(\'";
			_body += _uri + "/" + entry->d_name;
			if (_is_a_directory(_path_to_root + _uri + "/" + entry->d_name + "/"))
				_body += "/";
			_body += "\t\')\"><b>Delete</b></button>\n\t</li>\n";
		}
	}
	_body += "\t</ul>\n";
	if (std::find(_route->accepted_methods.begin(), _route->accepted_methods.end(), "POST") !=
			_route->accepted_methods.end())
	{
		_body += "\t<h2> Upload a new file </h2>\n"
						 "\t<form id=\"uploadForm\" onsubmit=\"uploadFile(event)\">\n"
						 "\t\t<input type=\"file\" id=\"fileInput\" name=\"file\">\n"
						 "\t\t<button type=\"submit\">Upload</button>\n"
						 "\t</form>\n";
	}
	_body += "</body>\n</html>";

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
	const char *path;

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
	set_status_message(_error_codes[num]);
	set_headers("Content-Type", "text/html");
	_body = "<h1>" + to_string(num) + " " + _error_codes[num] + "</h1>\n";
	set_content_lenght();
}
