#include "Response.hpp"

void Response::set_response_buffer(const char *response_buffer)
{
	_response_buffer = const_cast<char *>(response_buffer);
}

void Response::set_response_size(const size_t &response_size)
{
	_response_size = response_size;
}
