/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wmari <wmari@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 14:52:17 by wmari             #+#    #+#             */
/*   Updated: 2024/02/16 15:24:12 by wmari            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

void handle_co(int client_sock)
{
	std::string inside_sock;
}

int main()
{
	int server_fd;
	int new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	const int port = 4243;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) == -1)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	std::cout << "Server listening on port " << port << std::endl;

	while(1)
	{
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen )) < 0)
		{
			perror("accept failed");
			exit(EXIT_FAILURE);
		}
		
	}

	return 0;
}