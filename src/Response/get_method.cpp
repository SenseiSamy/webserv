#include "Response.hpp"
#include <limits.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>

void Response::directory_listing() {

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

bool Response::is_a_directory() {
	struct stat statbuf;
	if (stat(_url.c_str(), &statbuf) != 0)
		return (false);
	return (S_ISDIR(statbuf.st_mode) != 0);
}

// checks if the file exists and is readable, gets rid of the . and ..,
// and checks if the path is still within the server directory
int Response::check_and_rewrite_url()
{
	if (access(_url.c_str(), F_OK) == -1)
		return (404);
	if (access(_url.c_str(), R_OK) == -1)
		return (403);

	char path[PATH_MAX];
	if (realpath(_url.c_str(), path) == NULL)
		return (500);
	_url = path;
	if (getcwd(path, PATH_MAX) == NULL)
		return (500);
	std::string cwd = std::string(path) + "/www";
	if (_url.find(cwd) != 0)
		return (403);
	_url.erase(0, std::string(cwd).size() + 1);
	_url = "www/" + _url;
	return (0);
}

void Response::get_handler()
{
	if (_route != NULL && !_route->redirect.empty()) {
		redirect();
		return;
	}

	if (get_request().get_url() == "/")
		_url = "www/index.html";
	else
		_url = "www" + _request.get_url();

	if (_route != NULL && is_a_directory()) {
		if (!_route->index.empty()) {
			_url = "www/" + _route->index;
		}
		else {
			if (_route->autoindex)
				directory_listing();
			else
				generateHTTPError(404);
			return;
		}
	}

	find_type();
	int err = check_and_rewrite_url();
	if (err == 0) {
		std::ifstream file(_url.c_str());
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