#include "Response.hpp"

#include <fstream>

void Response::_get()
{
	if (_route != NULL && !_route->redirect.empty())
	{
		_redirect();
		return;
	}

	if (get_request().get_uri() == "/")
		_uri = "/index.html";
	else if (_uri.empty())
		_uri = _request.get_uri();

	if (_route != NULL && _is_a_directory(_path_to_root + _uri))
	{
		if (!_route->default_file.empty())
			_uri = "/" + _route->default_file;
		else
		{
			if (_route->directory_listing)
				_directory_listing();
			else
				_generate_response_code(404);
			return;
		}
	}

	int status_code = _cgi(-1);
	if (status_code == 1)
		return;
	if (status_code > 1)
	{
		_is_cgi = false;
		_generate_response_code(status_code);
		return;
	}

	_find_type();
	int err = _check_and_rewrite_uri();
	if (err == 0)
	{
		std::ifstream file((_path_to_root + _uri).c_str());
		if (!file.is_open())
		{
			_generate_response_code(500);
			return;
		}
		set_status_code(200);
		set_status_message(_error_codes[200]);
		// _add_content_type();
		std::string line;
		while (std::getline(file, line))
			_body += line + "\n";
		_add_content_type();
		set_content_lenght();
	}
	else
		_generate_response_code(err);
}
