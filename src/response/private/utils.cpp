#include "Response.hpp"

/* Types */
#include <limits.h> // PATH_MAX

/* Functions */
#include <sys/stat.h> // stat

const std::string Response::_real_path(const std::string &path)
{
	const std::string absolute_path = _server.get_server().root + path;
	char resolved_path[PATH_MAX];

	if (realpath(absolute_path.c_str(), resolved_path) == NULL)
		return "";

	return resolved_path;
}

/**
 * Determines the type of the file specified by the given file path.
 *
 * @param file_path The path of the file to check.
 * @return Returns 0 if the file is a regular file, 1 if it is a directory, or -1 if there was an error.
 */
int Response::_get_file_type(const std::string &file_path) const
{
	struct stat file_stat;

	if (stat(file_path.c_str(), &file_stat) == -1)
		return -1;

	if (S_ISDIR(file_stat.st_mode))
		return 1; // Directory
	else if (S_ISREG(file_stat.st_mode))
		return 0; // Regular file

	return -1; // Error
}

void Response::_set_content_type(const std::string &file_extension)
{
	if (file_extension == ".html" || file_extension == ".htm")
		_headers["Content-Type"] = "text/html";
	else if (file_extension == ".css")
		_headers["Content-Type"] = "text/css";
	else if (file_extension == ".js")
		_headers["Content-Type"] = "application/javascript";
	else if (file_extension == ".jpg" || file_extension == ".jpeg")
		_headers["Content-Type"] = "image/jpeg";
	else if (file_extension == ".png")
		_headers["Content-Type"] = "image/png";
	else if (file_extension == ".gif")
		_headers["Content-Type"] = "image/gif";
	else
		_headers["Content-Type"] = "application/octet-stream";
}

void Response::_set_status(const std::string &status_code, const std::string &status_message)
{
	_status_code = status_code;
	_status_message = status_message;
}

void Response::_read_file_content(const std::string &file_path)
{
	std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		_body = content;
		_content_length = content.size();
		file.close();
	}
	else
		generate_error_page(404);
}

const route *Response::_find_route() const
{
	const std::vector<route> &routes = _server.get_server().routes;

	for (size_t i = 0; i < routes.size(); ++i)
	{
		if (_request.get_uri().find(routes[i].path) == 0)
		{
			for (size_t j = 0; j < routes[i].accepted_methods.size(); ++j)
			{
				if (_request.get_method() == routes[i].accepted_methods[j])
					return &routes[i];
			}
		}
	}

	return NULL;
}
