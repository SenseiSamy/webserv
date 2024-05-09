#include "Cgi.hpp"
#include "Response.hpp"
#include <cstddef>
#include <string>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <dirent.h>

bool Response::is_a_directory(std::string url) {
	struct stat statbuf;
	if (stat(url.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISDIR(statbuf.st_mode));
}

bool Response::is_a_file(std::string url) {
	struct stat statbuf;
	if (stat(url.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISREG(statbuf.st_mode));
}

bool Response::is_cgi_request() {
	if (_route == NULL || _route->cgi.empty())
		return (false);
	for (std::map<std::string, std::string>::const_iterator it = _route->cgi.
	begin(); it != _route->cgi.end(); ++it) {
		std::size_t i = _url.find(it->first);
		while (i != std::string::npos) {
			if (is_a_file(_path_to_root + _url.substr(0, i + it->first.size())))
				break;
			i = _url.find(it->first, i + it->first.size());
		}
		if (i != std::string::npos)
		{
			std::string rep;
			std::string url = _url.substr(0, i + it->first.size());
			std::string path_info = _url.substr(i + it->first.size(),
				std::string::npos);
			_is_cgi = true;
			if (Cgi::handle_cgi(_request, rep, url, _path_to_root, it->second) != 0) {
				generateHTTPError(500);
				return (true);
			}
			setStatusCode(200);
			setStatusMessage(_hec.get_description(200));
			_body += rep;
			set_content_lenght();
			return (true);
		}
	}
	return (false);
}

// bool Response::handle_cgi_request() {
// 	if (_route == NULL || _route->cgi.empty())
// 		return (false);
// 	std::size_t extension_pos = _url.find(".");
// 	if (extension_pos == std::string::npos)
// 		return (false);
// 	std::string extension = _url.substr(extension_pos, std::string::npos);
// 	std::map<std::string, std::string>::iterator it;
// 	it = _route->cgi.find(extension);
// 	if (it == _route->cgi.end())
// 		return (false);

// 	std::string rep;
// 	if (Cgi::handle_cgi(_request, rep, _url, _path_to_root, it->second) != 0) {
// 		generateHTTPError(500);
// 		return (true);
// 	}
// 	_is_cgi = true;
// 	setStatusCode(200);
// 	setStatusMessage(_hec.get_description(200));
// 	_body += rep;
// 	set_content_lenght();
// 	return (true);
// }

void Response::directory_listing() {
	struct dirent* entry;
	DIR* directory = opendir((_path_to_root +_url).c_str());
	if (directory == NULL) {
		generateHTTPError(500);
		return;
	}

	setStatusCode(200);
	setStatusMessage(_hec.get_description(200));
	set_headers("Content-Type", "text/html");

	_body += "<ul>\n";
	_body += "<li><a href=..>..</a></li?>";
	while ((entry = readdir(directory)) != NULL) {
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

void Response::redirect() {
	if (_route == NULL || _route->redirect.empty()) {
		generateHTTPError(500);
		return;
	}
	setStatusCode(307);
	setStatusMessage(_hec.get_description(307));
	set_headers("Location", _route->redirect);
}

// checks if the file exists and is readable, gets rid of the . and ..,
// and checks if the path is still within the server directory
int Response::check_and_rewrite_url()
{
	if (access((_path_to_root + _url).c_str(), F_OK) == -1)
		return (404);
	if (access((_path_to_root + _url).c_str(), R_OK) == -1)
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

void Response::get_handler()
{
	if (_route != NULL && !_route->redirect.empty()) {
		redirect();
		return;
	}

	if (get_request().get_url() == "/")
		_url = "/index.html";
	else if (_url.empty())
		_url = _request.get_url();

	if (_route != NULL && is_a_directory(_path_to_root + _url)) {
		if (!_route->index.empty()) {
			_url = "/" + _route->index;
		}
		else {
			if (_route->autoindex)
				directory_listing();
			else
				generateHTTPError(404);
			return;
		}
	}

	if (is_cgi_request())	
		return;
	find_type();
	int err = check_and_rewrite_url();
	if (err == 0) {
		if (is_cgi_request())
			return;
		std::ifstream file((_path_to_root + _url).c_str());
		if (!file.is_open()) {
			generateHTTPError(500);
			return;
		}
		setStatusCode(200);
		setStatusMessage(_hec.get_description(200));
		add_content_type();
		std::string line;
		while (std::getline(file, line))
			_body += line + "\n";
		add_content_type();
		set_content_lenght();
	}
	else
		generateHTTPError(err);
}