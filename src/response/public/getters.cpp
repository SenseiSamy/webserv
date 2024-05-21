#include "Response.hpp"

const Server &Response::get_server() const
{
	return _server;
}

const Request &Response::get_request() const
{
	return _request;
}

const char *Response::get_response_buffer() const
{
	return _response_buffer;
}

const size_t &Response::get_response_size() const
{
	return _response_size;
}
