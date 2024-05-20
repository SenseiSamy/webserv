#include "Server.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

int main(int argc, const char *argv[])
{
	if (argc < 2 || argc > 3)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file> [--verbose|--help]" << std::endl;
		return EXIT_FAILURE;
	}

	int file_index = 1;
	bool verbose = false;

	for (int i = 1; argv[i]; ++i)
	{
		if (std::strcmp(argv[i], "--verbose") == 0 || std::strcmp(argv[i], "-v") == 0)
			verbose = true;
		else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0)
		{
			std::cout << "Usage: " << argv[0] << " <config_file> [--verbose|--help]" << std::endl;
			return EXIT_SUCCESS;
		}
		else
			file_index = i;
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
