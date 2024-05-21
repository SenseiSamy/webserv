#include "Parsing.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Utils.hpp"

/* Functions */
#include <cerrno>			 // errno
#include <iostream>		 // std::cerr, std::endl
#include <sys/epoll.h> // epoll_create1, epoll_ctl, epoll_event, epoll_wait
#include <unistd.h>		 // close

static std::vector<Server> init_servers(const char *config_file,
																				const std::map<unsigned short, std::string> &error_codes)
{
	std::vector<Server> servers;

	try
	{
		servers = parsing_config(config_file, error_codes);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return servers;
}

static int find_server(const std::vector<Server> &servers, int fd)
{
	for (int i = 0; i < servers.size(); ++i)
		if (servers[i].get_client_fd() == fd)
			return i;
	return -1;
}

int run(std::vector<Server> &servers, int epoll_fd, epoll_event *events,
				const std::map<unsigned short, std::string> &error_codes)
{
	while (true)
	{
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds == -1)
		{
			std::cerr << "Error: epoll_wait" << std::endl;
			return errno;
		}

		for (int i = 0; i < nfds; ++i)
		{
			const int server_index = find_server(servers, events[i].data.fd);
			if (server_index == -1) // new connection
			{
				int client_fd = servers[server_index].accept_client();
				if (client_fd == -1)
				{
					std::cerr << "Error: accept_client" << std::endl;
					return errno;
				}

				epoll_event event;
				event.events = EPOLLIN | EPOLLET;
				event.data.fd = client_fd;

				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
				{
					std::cerr << "Error: epoll_ctl" << std::endl;
					return errno;
				}
			}
			else // existing connection
			{
				Server &server = servers[server_index];
				Request request(server);
				Response response(server, request);
			}
		}
	}

	return 0;
}

int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		const std::map<unsigned short, std::string> error_codes = init_error_codes();

		std::vector<Server> servers = init_servers(argv[1], error_codes);

		/* Setup epoll*/
		int epoll_fd = epoll_create1(1);
		if (epoll_fd == -1)
		{
			std::cerr << "Error: epoll_create1" << std::endl;
			return EXIT_FAILURE;
		}

		for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
		{
			int listen_fd = it->setup_socket();
			if (listen_fd == -1)
			{
				std::cerr << "Error: setup_socket" << std::endl;
				return EXIT_FAILURE;
			}

			epoll_event event;
			event.events = EPOLLIN | EPOLLET;
			event.data.fd = listen_fd;

			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1)
			{
				std::cerr << "Error: epoll_ctl" << std::endl;
				return EXIT_FAILURE;
			}
		}

		/* Main loop */
		epoll_event events[MAX_EVENTS];
		run(servers, epoll_fd, events, error_codes);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
