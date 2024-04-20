#include "Cgi.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

Cgi::Cgi()
{
}

Cgi::Cgi(const Cgi &other)
{
	*this = other;
}

Cgi &Cgi::operator=(const Cgi &other)
{
	if (this != &other)
	{
	}
	return *this;
}

Cgi::~Cgi()
{
}

bool Cgi::is_cgi_request(std::string &request)
{
    return (request.compare(0, 13, "GET /cgi-bin/") == 0);
}

int Cgi::fork_and_exec(std::map<std::string, std::string> &meta_var, int *fd, int pid)
{
    if (pipe(fd) == -1)
        return (EXIT_FAILURE);
    pid = fork();
    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        char *args[] = {strdup("test"), NULL};
        std::string scriptPath = "www" + meta_var["SCRIPT_NAME"];
        execve(scriptPath.c_str(), args, map_to_env(meta_var));
        exit(EXIT_FAILURE);
    }
    else if (pid == -1)
        return (EXIT_FAILURE);
    close(fd[1]);
    return (EXIT_SUCCESS);
}

std::string Cgi::get_cgi_output(int *fd)
{
    std::string rep("HTTP/1.1 200 OK\n");
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

int Cgi::handle_cgi(std::string &request, int sock)
{
    std::map<std::string, std::string> meta_var = generate_meta_variables(request);
    std::string rep;
    int fd[2];
    int pid = 0;

    if (access(("www" + meta_var["SCRIPT_NAME"]).c_str(), F_OK | X_OK) == -1)
        return (EXIT_FAILURE);

    if (fork_and_exec(meta_var, fd, pid) == EXIT_FAILURE)
        return (EXIT_FAILURE);

    waitpid(pid, NULL, 0);

    rep = get_cgi_output(fd);
    if (rep.empty() || send(sock, rep.c_str(), rep.size(), 0) == -1)
        return (EXIT_FAILURE);

    return (EXIT_SUCCESS);
}
