#include "Response.hpp"
#include "Utils.hpp"

/* Functions */
#include <stdlib.h>

static char *strdup(const std::string &str)
{
	char *new_str = new char[str.size()];
	std::strcpy(new_str, str.c_str());
	return (new_str);
}

int Response::_exec_cgi(int *fd, int &pid, const std::string &path_to_exec_prog)
{
	if (pipe(fd) == -1)
		return (EXIT_FAILURE);
	pid = fork();
	if (pid == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		_cgi_script_path = _real_path_root + _request.get_uri();
		if (path_to_exec_prog.empty())
		{
			char *args[] = {strdup(_cgi_script_path), NULL};
			char **envp = to_cstr_array(_env);
			execve(_cgi_script_path.c_str(), args, envp);
			for (int i = 0; envp[i]; ++i)
				delete envp[i];

			delete envp;
			delete args[0];
		}
		else
		{
			char *args[] = {strdup(path_to_exec_prog), dup(_cgi_script_path), NULL};
			char **envp = to_cstr_array(_env);
			execve(path_to_exec_prog.c_str(), args, envp);
			for (int i = 0; envp[i]; ++i)
				delete envp[i];
			delete envp;
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

int Response::_cgi_request(std::string &rep, std::string path_to_exec_prog)
{
	_init_env();
	int fd[2];
	int saved_fd;
	int fd_data;
	int pid = 0;

	if (_request.get_method() == "POST" &&
			_request.get_headers_key("Content-Type") == "application/x-www-form-urlencoded")
	{
		saved_fd = dup(STDIN_FILENO);
		fd_data = open(_request.get_file_name().c_str(), O_RDONLY);
		if (fd_data == -1)
			return (500);
		dup2(fd_data, STDIN_FILENO);
		close(fd_data);
	}
	if (access((_real_path_root + _request.get_uri()).c_str(), F_OK) == -1)
		return (404);
	if (path_to_exec_prog.empty() && access((_real_path_root + _request.get_uri()).c_str(), X_OK) == -1)
		return (403);

	if (_exec_cgi(fd, pid, path_to_exec_prog) == 1)
	{
		dup2(saved_fd, STDIN_FILENO);
		close(saved_fd);
		return (EXIT_FAILURE);
	}

	waitpid(pid, NULL, 0);
	dup2(saved_fd, STDIN_FILENO);
	close(saved_fd);
	rep = _get_cgi_output(fd);

	return (EXIT_SUCCESS);
}

void Response::_init_env()
{
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::string tmp = it->first;
		for (std::string::iterator c = tmp.begin(); c != tmp.end(); ++c)
		{
			if (*c == '-')
				*c = '_';
			else
				*c = std::toupper(*c);
		}
		_env["HTTP_" + tmp] = "[" + it->second + "]";
	}

	_env["REQUEST_METHOD"] = _request.get_method();
	_env["REQUEST_URI"] = _request.get_uri();
	_env["SCRIPT_NAME"] = _path_file;
	if (_request.get_uri().find('?') == std::string::npos)
		_env["QUERY_STRING"] = "";
	else
		_env["QUERY_STRING"] = _request.get_uri().substr(_request.get_uri().find('?') + 1, std::string::npos);
	_env["AUTH_TYPE"] = ""; // Implement based on specific auth handling
	_env["CONTENT_LENGTH"] = get_headers_key("Content-Length");
	_env["CONTENT_TYPE"] = get_headers_key("Content-Type");
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	if (_path_file.size() < _request.get_uri().size())
		_env["PATH_INFO"] = _request.get_uri().substr(_path_file.size(), std::string::npos);
	else
		_env["PATH_INFO"] = "";

	_env["PATH_TRANSLATED"] = ""; // Translate PATH_INFO to filesystem path
	_env["REMOTE_ADDR"] = "";			// To be retrieved from socket information
	_env["REMOTE_HOST"] = "";			// Optional: resolve REMOTE_ADDR to hostname
	_env["REMOTE_USER"] = "";			// User from authenticated session
	_env["SERVER_NAME"] = "";			// Retrieved from server configuration or host header
	_env["SERVER_PORT"] = "";			// Retrieved from server configuration
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "webserv-42/1.0";
}

int Response::_is_cgi()
{
	if (_route == NULL || _route->cgi.empty())
		return (1);
	for (std::map<std::string, std::string>::const_iterator it = _route->cgi.begin(); it != _route->cgi.end(); ++it)
	{
		std::string::size_type i = _path_file.find(it->first);
		while (i != std::string::npos)
		{
			if (_get_file_type(_real_path_root + _path_file.substr(0, i + it->first.size())) == 0)
				break;
			i = _path_file.find(it->first, i + it->first.size());
		}
		if (i != std::string::npos)
		{
			std::string rep;
			std::string uri = _path_file.substr(0, i + it->first.size());
			std::string path_info = _path_file.substr(i + it->first.size(), std::string::npos);
			_is_cgi = true;
			if (_cgi_handle(rep, it->second) != 0)
			{
				_set_(500);
				return (0);
			}
			set_status_code(200);
			set_status_message(_error_codes[200]);
			_body += rep;
			set_content_lenght();
			return (0);
		}
	}
	return (1);
}