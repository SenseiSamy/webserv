#include "Response.hpp"

/* Functions */
#include <dirent.h> // opendir, readdir, closedir
#include <linux/limits.h>

void Response::_get(const Request &request)
{
	if (_route != NULL && _route->default_file != "" && _file_type == 0)
		_file_path += _route->default_file;

	if (_route != NULL && !_route->redirect.empty()) // redirect
	{
		_server.set_status_code(307);
		set_header("Location", _route->redirect);
		return;
	}

	// directory listing
	if (_route != NULL && _file_type == 0)
	{
		if (!_route->directory_listing)
		{
			_server.set_status_code(403);
			return;
		}
		_content_type = "text/html";
		_body = "<html><head><title>Index of " + _file_path + "</title></head><body><h1>Index of " + _file_path +
						"</h1><hr><pre>";

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(_real_path_root.c_str())) != NULL)
		{
			while ((ent = readdir(dir)) != NULL)
				_body += "<a href=\"" + _file_path + ent->d_name + "\">" + ent->d_name + "</a><br>";
			closedir(dir);
		}
		_body += "</pre><hr></body></html>";
		_server.set_status_code(200);
		return;
	}

	// cgi
	// if (_is_cgi())
	// {
	// 	_cgi(_file_path);
	// 	return;
	// }

	_real_path_file = _real_path_root + _file_path;
	if (access(_real_path_file.c_str(), F_OK) == -1)
		_server.set_status_code(404);
	else if (_file_type <= 0 || access(_real_path_file.c_str(), R_OK) == -1)
		_server.set_status_code(403);

	if (_server.has_error())
		return;
	char buffer[PATH_MAX];
	if (realpath(_real_path_file.c_str(), buffer) == NULL)
		_server.set_status_code(500);

	_set_content_type(_file_path.substr(_file_path.find_last_of('.') + 1));
	_set_content_length();
	_set_status("200", "OK");
	_read_file_content(_real_path_file);
}
