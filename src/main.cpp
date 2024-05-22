#include "Parsing.hpp"
#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <iostream> // std::cerr, std::endl

int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return EXIT_FAILURE;
	}

	const std::map<unsigned short, std::string> error_codes = init_error_codes();

	std::vector<Server> servers = parsing_config(argv[1], error_codes);

	for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server)
		server->run();

	return EXIT_SUCCESS;
}
