#include "Parsing.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <iostream>
#include <map>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return EXIT_FAILURE;
	}

	std::map<unsigned short, std::string> error_codes = init_error_codes();
	std::vector<Server> servers = Parsing::parsing_config(argv[1], error_codes);

	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		perror("epoll_create1");
		return EXIT_FAILURE;
	}

	struct epoll_event ev, events[MAX_EVENTS];

	// Initialize servers and add their file descriptors to epoll
	for (size_t i = 0; i < servers.size(); ++i)
	{
		servers[i].init();
		ev.events = EPOLLIN;
		ev.data.fd = servers[i].get_server_fd();
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].get_server_fd(), &ev) == -1)
		{
			perror("epoll_ctl: server_fd");
			close(servers[i].get_server_fd());
			return EXIT_FAILURE;
		}
	}

	while (true)
	{
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (event_count == -1)
		{
			perror("epoll_wait");
			close(epoll_fd);
			return EXIT_FAILURE;
		}

		for (int i = 0; i < event_count; ++i)
		{
			int fd = events[i].data.fd;
			bool is_server_fd = false;

			// Check if the event is on one of the server sockets
			for (size_t j = 0; j < servers.size(); ++j)
			{
				if (fd == servers[j].get_server_fd())
				{
					servers[j].accept_connection(epoll_fd);
					is_server_fd = true;
					break;
				}
			}

			// If not a server socket, it must be a client socket
			if (!is_server_fd)
			{
				for (size_t j = 0; j < servers.size(); ++j)
					servers[j].handle_client(fd);
			}
		}
	}

	close(epoll_fd);
	return EXIT_SUCCESS;
}
