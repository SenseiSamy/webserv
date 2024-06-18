#include "Response.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

static inline std::string to_string(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

static char *strdup(const std::string &str)
{
	char *new_str = new char[str.size()];
	std::strcpy(new_str, str.c_str());
	return (new_str);
}

int Response::_fork_and_exec(int *fd, int &pid, std::string path_to_exec_prog, int fd_in)
{
	if (pipe(fd) == -1)
		return (EXIT_FAILURE);
	pid = fork();
	if (pid == 0)
	{
		if (fd_in > 0)
			dup2(fd_in, STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		std::string scriptPath = _path_to_root + _uri;
		if (path_to_exec_prog.empty())
		{
			char *args[] = {strdup(scriptPath), NULL};
			char **envp = _map_to_env();
			execve(scriptPath.c_str(), args, envp);
			for (int i = 0; envp[i]; ++i)
				delete envp[i];

			delete[] envp;
			delete args[0];
		}
		else
		{
			char *args[] = {strdup(path_to_exec_prog), strdup(scriptPath), NULL};
			char **envp = _map_to_env();
			execve(path_to_exec_prog.c_str(), args, envp);
			for (int i = 0; envp[i]; ++i)
				delete envp[i];
			delete[] envp;
			delete args[0];
			delete args[1];
		}
		std::exit(EXIT_FAILURE);
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

int Response::_cgi_request(std::string &rep, std::string path_to_exec_prog, int fd_in)
{
	_init_meta_var();
	int fd[2];
	int pid = 0;

	if (access((_path_to_root + _uri).c_str(), F_OK) == -1)
		return (404); // Not Found
	if (path_to_exec_prog.empty() && access((_path_to_root + _uri).c_str(), X_OK) == -1)
		return (403); // Forbidden

	clock_t timer = clock();
	if (_fork_and_exec(fd, pid, path_to_exec_prog, fd_in) == 1)
		return (500); // Internal Server Error

	int p;
	while ((p = waitpid(pid, NULL, WNOHANG)) == 0)
	{
		if (p == -1)
			return (500); // Internal Server Error
		if ((((double)(clock() - timer)) / CLOCKS_PER_SEC) > 10.0)
		{
			kill(pid, SIGKILL);
			return (504); // Gateway Timeout
		}
	}
	rep = _get_cgi_output(fd);
	return (EXIT_SUCCESS);
}

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

static void header_to_var(char &c)
{
	if (c == '-')
		c = '_';
	else
		c = std::toupper(c);
}

void Response::_init_meta_var()
{
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::string tmp = it->first;
		std::for_each(tmp.begin(), tmp.end(), &header_to_var);
		_meta_var["HTTP_" + tmp] = "[" + it->second + "]";
	}

	_meta_var["REQUEST_METHOD"] = _request.get_method();
	_meta_var["REQUEST_URI"] = _request.get_uri();
	_meta_var["SCRIPT_NAME"] = _uri;
	_meta_var["QUERY_STRING"] =
			(_request.get_uri().find('?') == std::string::npos ? ""
																												 : _request.get_uri().substr(_request.get_uri().find('?') + 1));
	_meta_var["AUTH_TYPE"] = ""; // Implement based on specific auth handling
	_meta_var["CONTENT_LENGTH"] = get_headers_key("Content-Length");
	_meta_var["CONTENT_TYPE"] = get_headers_key("Content-Type");
	_meta_var["GATEWAY_INTERFACE"] = "CGI/1.1";
	if (_uri.size() < _request.get_uri().size())
		_meta_var["PATH_INFO"] = _request.get_uri().substr(_uri.size(), std::string::npos);
	else
		_meta_var["PATH_INFO"] = "";

	_meta_var["PATH_TRANSLATED"] = ""; // Translate PATH_INFO to filesystem path
	_meta_var["REMOTE_ADDR"] = "";		 // To be retrieved from socket information
	_meta_var["REMOTE_HOST"] = "";		 // Optional: resolve REMOTE_ADDR to hostname
	_meta_var["REMOTE_USER"] = "";		 // User from authenticated session
	_meta_var["SERVER_NAME"] = "";		 // Retrieved from server configuration or host header
	_meta_var["SERVER_PORT"] = "";		 // Retrieved from server configuration
	_meta_var["SERVER_PROTOCOL"] = "HTTP/1.1";
	_meta_var["SERVER_SOFTWARE"] = "webserv-42/1.0";
}

int Response::_cgi(int fd_in)
{
	if (_route == NULL || _route->cgi.empty())
		return (false);
	for (std::map<std::string, std::string>::const_iterator it = _route->cgi.begin(); it != _route->cgi.end(); ++it)
	{
		std::size_t i = _uri.find(it->first);
		while (i != std::string::npos)
		{
			if (_is_a_file(_path_to_root + _uri.substr(0, i + it->first.size())))
				break;
			i = _uri.find(it->first, i + it->first.size());
		}
		if (i != std::string::npos)
		{
			std::string rep;
			std::string uri = _uri.substr(0, i + it->first.size());
			std::string path_info = _uri.substr(i + it->first.size());
			_is_cgi = true;
			int status = _cgi_request(rep, it->second, fd_in);
			if (status != EXIT_SUCCESS)
			{
				set_status_code(status);
				set_status_message(_error_codes[status]);
				_body = "\r\n<h1>" + to_string(status) + " " + _error_codes[status] + "</h1>\n";
				return (true);
			}
			set_status_code(200);
			set_status_message(_error_codes[200]);
			_body += rep;
			set_content_lenght();
			return (true);
		}
	}
	return (false);
}
