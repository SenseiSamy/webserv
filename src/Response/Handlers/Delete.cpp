#include "Response.hpp"

#include <cstdio>
#include <string>

void Response::_delete()
{
	_uri = _request.get_uri();
	std::string filename(_path_to_root + _uri);
	if (_is_a_directory(filename))
	{
		if (filename.at(filename.size() - 1) != '/')
			_generate_error(403);
		else if (_write_perm(filename))
			_generate_error(403);
		else if (!std::remove(filename.c_str()))
		{
			set_status_code(204);
			return;
		}
		else
			_generate_error(500);
	}
	else
	{
		if (_write_perm(filename))
			_generate_error(401);
		else if (!_exists(filename))
			_generate_error(404);
		else if (!std::remove(filename.c_str()))
		{
			set_status_code(204);
			return;
		}
		else
			_generate_error(500);
	}
}