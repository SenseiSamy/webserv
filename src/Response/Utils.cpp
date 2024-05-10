#include "Response.hpp"

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

std::string Response::_to_string(size_t i) const
{
	std::ostringstream oss;
	oss << i;
	return oss.str();
}

std::string Response::_concatenate_paths(const std::string &base, const std::string &relative)
{
	std::vector<std::string> base_tokens;
	std::vector<std::string> relative_tokens;
	std::vector<std::string> result_tokens;

	std::istringstream base_stream(base);
	std::string token;
	while (std::getline(base_stream, token, '/'))
	{
		if (!token.empty())
			base_tokens.push_back(token);
	}

	std::istringstream relative_stream(relative);
	while (std::getline(relative_stream, token, '/'))
	{
		if (!token.empty())
			relative_tokens.push_back(token);
	}

	for (std::vector<std::string>::const_iterator it = base_tokens.begin(); it != base_tokens.end(); ++it)
	{
		if (*it == "..")
		{
			if (!result_tokens.empty())
				result_tokens.pop_back();
		}
		else
			result_tokens.push_back(*it);
	}

	for (std::vector<std::string>::const_iterator it = relative_tokens.begin(); it != relative_tokens.end(); ++it)
	{
		if (*it == "..")
		{
			if (!result_tokens.empty())
				result_tokens.pop_back();
		}
		else
			result_tokens.push_back(*it);
	}

	std::string result;
	for (std::vector<std::string>::const_iterator it = result_tokens.begin(); it != result_tokens.end(); ++it)
		result += *it + '/';

	return result;
}

std::string Response::_sanitize_url(const std::string &url)
{
	if (_server.root.empty())
		_generate_response_body(500);
	
	if (url.empty())
		return _server.default_file.empty() ? "index.html" : _server.default_file;

	std::string path = _concatenate_paths(_server.root, url);
	if (path.find(_server.root) != 0) // Si le chemin ne commence pas par le chemin racine du serveur
		return _server.default_file.empty() ? "index.html" : _server.default_file;

	return path;
}


const std::string Response::_mime_type(const std::string &file) const
{
	const std::string extension = file.substr(file.find_last_of(".") + 1);

	if (extension == "html")
		return "text/html";
	else if (extension == "css")
		return "text/css";
	else if (extension == "jpg" || extension == "jpeg")
		return "image/jpeg";
	else if (extension == "ico")
		return "image/x-icon";
	else if (extension == "png")
		return "image/png";
	else
		return "application/octet-stream";
}

void Response::_generate_response(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

	if (file)
	{
		std::ostringstream contents;
		contents << file.rdbuf();
		file.close();

		_body = contents.str();
		_status_code = 200;
		_status_message = _error_map[_status_code];
		_headers["Content-Length"] = _to_string(_body.size());
		_headers["Content-Type"] = _mime_type(path);
	}
	else
		_generate_response_body(404);
}

void Response::_generate_response_body(unsigned short error_code)
{
	_status_code = error_code;
	_status_message = _error_map[_status_code];
	_body = "<html><head><title>" + _status_message + "</title></head><body><center><h1>" + _status_message + "</h1></center></body></html>";
	_headers["Content-Length"] = _to_string(_body.size());
	_headers["Content-Type"] = "text/html";
}

int Response::_is_directory_or_file(const std::string &path) const
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return -1;	// Erreur lors de la récupération des informations

	if (S_ISDIR(statbuf.st_mode))
		return 1;		// C'est un dossier
	else if (S_ISREG(statbuf.st_mode))
		return 0;		// C'est un fichier
	else
		return -2;	// Le chemin n'est ni un fichier ni un dossier
}
