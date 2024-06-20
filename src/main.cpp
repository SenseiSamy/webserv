#include "Server.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

int main(int argc, const char *argv[])
{
	if (argc < 2 || argc > 3)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file> [--verbose|--help]" << std::endl;
		return EXIT_FAILURE;
	}

	int file_index = 1;
	bool verbose = false;

	if (argc == 3)
	{
		std::string flag = argv[2];
		if (flag == "--verbose" || flag == "-v")
			verbose = true;
	}

	try
	{
		Server server(argv[file_index], verbose);
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
