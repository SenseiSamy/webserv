#include "Response.hpp"

Response::Response(Server &_server, const Request &request) : _server(_server), _request(request)
{
	_response_buffer = NULL;
	_response_size = 0;

	_build_response(request);
	_send_response();
}
