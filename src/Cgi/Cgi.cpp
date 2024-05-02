#include "Cgi.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

Cgi::Cgi()
{
}

Cgi::Cgi(const Cgi& other)
{
	*this = other;
}

Cgi& Cgi::operator=(const Cgi& other)
{
	if (this != &other)
	{
	}
	return (*this);
}

Cgi::~Cgi()
{
}

int Cgi::fork_and_exec(std::map<std::string, std::string>& meta_var, int* fd,
	int& pid, std::string path_to_root, std::string path_to_exec_prog,
	std::string url)
{
	if (pipe(fd) == -1)
		return (EXIT_FAILURE);
	pid = fork();
	if (pid == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		std::string scriptPath = path_to_root + url;
		if (path_to_exec_prog.empty()) {
			char* args[] = {strdup(scriptPath.c_str()), NULL};
			execve(scriptPath.c_str(), args, map_to_env(meta_var));
		}
		else {
			char* args[] = {strdup(path_to_exec_prog.c_str()),
				strdup(scriptPath.c_str()), NULL};
			execve(path_to_exec_prog.c_str(), args, map_to_env(meta_var));
		}
		exit(EXIT_FAILURE);
	}
	else if (pid == -1)
		return (EXIT_FAILURE);
	close(fd[1]);
	return (EXIT_SUCCESS);
}

std::string Cgi::get_cgi_output(int* fd)
{
	std::string rep;
	char buf[128];
	int size;

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

int Cgi::handle_cgi(const Request& request, std::string &rep, std::string url,
	std::string path_to_root, std::string path_to_exec_prog)
{
	std::map<std::string, std::string> meta_var = generate_meta_variables(request);
	int fd[2];
	int pid = 0;

	if (access((path_to_root + url).c_str(), F_OK) == -1)
		return (404);
	if (path_to_exec_prog.empty() && access((path_to_root + url).c_str(), X_OK)
	== -1)
		return (403);

	if (fork_and_exec(meta_var, fd, pid, path_to_root, path_to_exec_prog, url)
		== EXIT_FAILURE)
		return (EXIT_FAILURE);

	waitpid(pid, NULL, 0);

	rep = get_cgi_output(fd);

	return (EXIT_SUCCESS);
}
