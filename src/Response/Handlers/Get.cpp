#include "Response.hpp"

#include <fstream>

void Response::_get()
{
	if (_route != NULL && !_route->redirect.empty())
	{
		_redirect();
		return;
	}

	if (get_request().get_url() == "/")
		_url = "/index.html";
	else if (_url.empty())
		_url = _request.get_url();

	if (_route != NULL && _is_a_directory(_path_to_root + _url))
	{
		if (!_route->default_file.empty())
			_url = "/" + _route->default_file;
		else
		{
			if (_route->_directory_listing)
				_directory_listing();
			else
				_generate_error(404);
			return;
		}
	}

	if (_is_cgi_request())	
		return;
	_find_type();
	int err = _check_and_rewrite_url();
	if (err == 0) {
		std::ifstream file((_path_to_root + _url).c_str());
		if (!file.is_open())
		{
			_generate_error(500);
			return;
		}
		setStatusCode(200);
		setStatusMessage(_error_codes[200]);
		_add_content_type();
		std::string line;
		while (std::getline(file, line))
			_body += line + "\n";
		_add_content_type();
		set_content_lenght();
	}
	else
		_generate_error(err);
}
