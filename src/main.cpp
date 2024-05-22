#include "Parsing.hpp"
#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <iostream>		 // std::cerr, std::endl
#include <sys/types.h> // pid_t
#include <sys/wait.h>	 // waitpid

void run_server(Server *server)
{
	server->run();
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

	std::vector<pid_t> child_pids;
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			std::cerr << "fork: failed to create child process" << std::endl;
			return EXIT_FAILURE;
		}
		else if (pid == 0) // Child process
		{
			run_server(&(*it));
			return EXIT_SUCCESS;
		}
		else // Parent process
			child_pids.push_back(pid);
	}

	// Wait for all child processes to finish
	int status;
	for (std::vector<pid_t>::iterator it = child_pids.begin(); it != child_pids.end(); ++it)
	{
		if (waitpid(*it, &status, 0) == -1)
		{
			std::cerr << "waitpid: failed to wait for child process" << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
