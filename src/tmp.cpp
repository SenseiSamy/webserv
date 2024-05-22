#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 10
#define READ_BUFFER 1024
#define RESPONSE "HTTP/1.0 200 OK\r\nContent-Length: 11\r\nContent-Type: text/plain\r\n\r\nhello world"

class Server
{
public:
	Server(const std::string &ip, int port);
	void run();

private:
	void set_nonblocking(int sockfd);
	int create_and_bind_socket(const std::string &ip, int port);

	int server_fd;
	int epoll_fd;
	struct sockaddr_in address;
};

Server::Server(const std::string &ip, int port)
{
	server_fd = create_and_bind_socket(ip, port);
	set_nonblocking(server_fd);

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		perror("epoll_create1");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
	{
		perror("epoll_ctl: server_fd");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
}

void Server::set_nonblocking(int sockfd)
{
	int opts = fcntl(sockfd, F_GETFL);
	if (opts < 0)
	{
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sockfd, F_SETFL, opts) < 0)
	{
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
}

int Server::create_and_bind_socket(const std::string &ip, int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip.c_str());
	address.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 10) < 0)
	{
		perror("listen failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

void Server::run()
{
	struct epoll_event events[MAX_EVENTS];
	while (true)
	{
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (event_count == -1)
		{
			perror("epoll_wait");
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i < event_count; i++)
		{
			if (events[i].data.fd == server_fd)
			{
				int client_fd = accept(server_fd, nullptr, nullptr);
				if (client_fd == -1)
				{
					perror("accept");
					continue;
				}
				set_nonblocking(client_fd);
				struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
				{
					perror("epoll_ctl: client_fd");
					close(client_fd);
				}
			}
			else
			{
				bool done = false;
				while (!done)
				{
					char buffer[READ_BUFFER];
					ssize_t count = read(events[i].data.fd, buffer, sizeof(buffer));
					if (count == -1)
					{
						if (errno != EAGAIN)
						{
							perror("read");
							close(events[i].data.fd);
							done = true;
						}
						break;
					}
					else if (count == 0)
					{
						close(events[i].data.fd);
						done = true;
						break;
					}

					write(events[i].data.fd, RESPONSE, strlen(RESPONSE));
					close(events[i].data.fd);
					done = true;
				}
			}
		}
	}
}

int main()
{
	Server server1("127.0.0.1", 8080);
	server1.run();

	// Pour ajouter un autre serveur, créez une autre instance de Server
	// Server server2("127.0.0.1", 8081);
	// server2.run();

	return 0;
}
