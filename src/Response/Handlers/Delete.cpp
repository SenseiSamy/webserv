#include "Response.hpp"

#include <string>
#include <sys/stat.h>

void Response::_delete()
{
	_uri = _request.get_uri();
	std::string filename(_path_to_root + _uri);
	if (_is_a_directory(filename))
	{
		if (filename.at(filename.size() - 1) != '/' || _write_perm(filename))
			_generate_response_code(403); // Forbidden
		else if (!std::remove(filename.c_str()))
		{
			set_status_code(204); // No Content
			set_status_message(_error_codes[204]);
			return;
		}
		else
			_generate_response_code(500); // Internal Server Error
	}
	else
	{
		if (_write_perm(filename))
			_generate_response_code(401); // Unauthorized
		else if (!_exists(filename))
			_generate_response_code(404); // Not Found
		else if (!std::remove(filename.c_str()))
		{
			set_status_code(204); // No Content
			set_status_message(_error_codes[204]);
			return;
		}
		else
			_generate_response_code(500); // Internal Server Error
	}
}