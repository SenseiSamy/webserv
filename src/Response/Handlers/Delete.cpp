#include "Response.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>
#include <string>
#include <sys/stat.h>

void	Response::_delete()
{
	std::string filename = _path_to_root + _url;
	if (_is_a_directory(filename))
	{
		if (filename.back() != '/')
		{
			set_status_code(409);
			set_status_message(_error_codes[409]);
			return ;
		}
		if (_write_perm(filename))
		{
			set_status_code(403);
			set_status_message(_error_codes[403]);
			return ;
		}
		if (!std::remove(filename.c_str()))
		{
			set_status_code(204);
			set_status_message(_error_codes[204]);
			return ;
		}
		else
		{
			set_status_code(500);
			set_status_message(_error_codes[500]);
			return ;
		}
	}
	else
	{
		if (!_exists(filename))
		{
			set_status_code(404);
			set_status_message(_error_codes[404]);
		}
		if (!std::remove(filename.c_str()))
		{
			set_status_code(204);
			set_status_message(_error_codes[204]);
		}
		else
		{
			set_status_code(401);
			set_status_message(_error_codes[401]);
		}
	}
}