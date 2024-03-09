/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 19:58:05 by snaji             #+#    #+#             */
/*   Updated: 2024/03/09 21:35:17 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <sys/socket.h>

std::map<std::string, std::string>	generateMetaVariables(std::string const& request);
char**	mapToEnv(std::map<std::string, std::string>& metaVar);

bool	isCGIrequest(std::string& request)
{
	return (request.compare(0, 13, "GET /cgi-bin/") == 0);
}

static int	fork_and_exec(std::map<std::string, std::string>& metaVar, int* fd, int pid)
{
	if (pipe(fd) == -1)
		return (EXIT_FAILURE);
	pid = fork();
	if (pid == 0)
	{
		dup2(fd[1], 1);
		execve(("www" + metaVar["SCRIPT_NAME"]).c_str(), (char *[]){strdup("test"), NULL}, mapToEnv(metaVar));	
		exit(EXIT_FAILURE);
	}
	else if (pid == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static std::string get_cgi_output(int *fd)
{
	std::string	rep("HTTP/1.1 200 OK\n");
	char		buf[128];
	int			size;

	close(fd[1]);
	while ((size = read(fd[0], buf, 127)) > 0)
	{
		buf[size] = 0;
		rep = rep + buf;
	}
	if (size == -1)
		return ("");
	buf[size] = 0;
	rep = rep + buf;
	close(fd[0]);
	return (rep);
}

int	handleCGI(std::string& request, int sock)
{
	std::map<std::string, std::string>	metaVar = generateMetaVariables(request);
	std::string	rep;
	int	fd[2];
	int	pid = 0;

	if (access(("www" + metaVar["SCRIPT_NAME"]).c_str(), F_OK | X_OK) == -1)
		return (EXIT_FAILURE);

	if (fork_and_exec(metaVar, fd, pid) == EXIT_FAILURE)
		return (EXIT_FAILURE);

	waitpid(pid, NULL, 0);

	rep = get_cgi_output(fd);
	if (rep.empty() || send(sock, rep.c_str(), rep.size(), 0) == -1)
		return (EXIT_FAILURE);

	return (EXIT_SUCCESS);
}
