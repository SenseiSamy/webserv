#include "Response.hpp"

#include <sys/wait.h>

char **Response::_map_to_env()
{
    char **env = new char *[_meta_var.size() + 1];
    std::map<std::string, std::string>::iterator it;
    std::size_t i;

    for (it = _meta_var.begin(), i = 0; it != _meta_var.end(); ++i, ++it)
    {
        std::string tmp = it->first + "=" + it->second;
        env[i] = new char[tmp.size() + 1];
        for (std::size_t j = 0; tmp.c_str()[j]; ++j)
            env[i][j] = tmp.c_str()[j];
        env[i][tmp.size()] = '\0';
    }
    env[i] = NULL;
    return (env);
}

void Response::_generate_meta_variables()
{
    std::map<std::string, std::string> meta_var;

    _meta_var["REQUEST_METHOD"] = _request.get_method();
    _meta_var["REQUEST_URI"] = _request.get_uri();
    _meta_var["SCRIPT_NAME"] = _request.get_uri().substr(0, _request.get_uri().find('?'));
    _meta_var["QUERY_STRING"] = (_request.get_uri().find('?') == std::string::npos
                                     ? ""
                                     : _request.get_uri().substr(_request.get_uri().find('?') + 1));

    const std::map<std::string, std::string> &header = _request.get_header();
    for (std::map<std::string, std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
        _meta_var["HTTP_" + it->first] = it->second;

    _meta_var["AUTH_TYPE"] = "";      // Implement based on specific auth handling
    _meta_var["CONTENT_LENGTH"] = ""; // Set in helper function
    _meta_var["CONTENT_TYPE"] = "";   // Set in helper function
    _meta_var["GATEWAY_INTERFACE"] = "CGI/1.1";
    _meta_var["PATH_INFO"] = "";       // Specific handling based on URL parsing
    _meta_var["PATH_TRANSLATED"] = ""; // Translate PATH_INFO to filesystem path
    _meta_var["REMOTE_ADDR"] = "";     // To be retrieved from socket information
    _meta_var["REMOTE_HOST"] = "";     // Optional: resolve REMOTE_ADDR to hostname
    _meta_var["REMOTE_USER"] = "";     // User from authenticated session
    _meta_var["SERVER_NAME"] = "";     // Retrieved from server configuration or host header
    _meta_var["SERVER_PORT"] = "";     // Retrieved from server configuration
    _meta_var["SERVER_PROTOCOL"] = "HTTP/1.1";
    _meta_var["SERVER_SOFTWARE"] = "webserv-42/1.0";
}

int Response::_fork_and_exec(int *fd, int &pid, std::string path_to_root, std::string path_to_exec_prog,
                             std::string url)
{
    if (pipe(fd) == -1)
        return (EXIT_FAILURE);
    pid = fork();
    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        std::string scriptPath = path_to_root + url;
        if (path_to_exec_prog.empty())
        {
            char *args[] = {strdup(scriptPath.c_str()), NULL};
            execve(scriptPath.c_str(), args, _map_to_env());
        }
        else
        {
            char *args[] = {strdup(path_to_exec_prog.c_str()), strdup(scriptPath.c_str()), NULL};
            execve(path_to_exec_prog.c_str(), args, _map_to_env());
        }
        exit(EXIT_FAILURE);
    }
    else if (pid == -1)
        return (EXIT_FAILURE);
    close(fd[1]);
    return (EXIT_SUCCESS);
}

std::string Response::_get_cgi_output(int *fd)
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

int Response::_cgi(std::string &rep, std::string url, std::string path_to_root, std::string path_to_exec_prog)
{
    _generate_meta_variables();
    int fd[2];
    int pid = 0;

    if (access((path_to_root + url).c_str(), F_OK) == -1)
        return (404);
    if (path_to_exec_prog.empty() && access((path_to_root + url).c_str(), X_OK) == -1)
        return (403);

    if (_fork_and_exec(fd, pid, path_to_root, path_to_exec_prog, url) == EXIT_FAILURE)
        return (EXIT_FAILURE);

    waitpid(pid, NULL, 0);

    rep = _get_cgi_output(fd);

    return (EXIT_SUCCESS);
}
