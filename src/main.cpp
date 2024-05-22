#include "Parsing.hpp"
#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <iostream>	 // std::cerr, std::endl
#include <pthread.h> // pthread_create, pthread_join

void *run_server(void *arg)
{
	Server *server = static_cast<Server *>(arg);
	server->run();
	return NULL;
}

int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return EXIT_FAILURE;
	}

	std::map<unsigned short, std::string> error_codes = init_error_codes();

	std::vector<Server> servers = Parsing::parsing_config(argv[1], error_codes);

	// create a thread for each server
	std::vector<pthread_t> threads;
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		pthread_t thread;
		if (pthread_create(&thread, NULL, &run_server, &(*it)) != 0)
		{
			std::cerr << "pthread_create: failed to create thread" << std::endl;
			return EXIT_FAILURE;
		}
		threads.push_back(thread);
	}

	return EXIT_SUCCESS;
}
