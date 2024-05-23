#include "Response.hpp"

Response::Response(Server &server, const Request &request)
		: _server(server), _request(request), _response_buffer(NULL), _response_size(0)
{
	// update_response_buffer();
	// send(_request.get_client_fd(), _response_buffer, _response_size, 0);
}

Response::~Response()
{
	if (_response_buffer)
		delete[] _response_buffer;
}

void Response::update_response_buffer()
{
}

void Response::generate_error_page(const unsigned short &status_code)
{
}