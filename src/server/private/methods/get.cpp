#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"

/* Functions */
#include <fstream> // std::ifstream
#include <sstream> // std::ostringstream

void Server::_get(const Request &request, Response &response)
{
	if (response.get_route() != NULL && ~response.get_route()->redirect.empty())
	{
		set_status_code(307); // Temporary Redirect
		response.set_header("Location", response.get_route()->redirect);
	}


	if (response.get_route() != NULL && response.get_file_type() == 1)
	{
		if (!response.get_route()->default_file.empty())
			
	}
}
