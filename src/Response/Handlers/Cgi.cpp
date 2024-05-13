#include "Response.hpp"

#include <algorithm>
#include <sys/wait.h>

static char* strdup(std::string str)
{
	char *new_str = new char[str.size()];
	std::strcpy(new_str, str.c_str());
	return (new_str);
}

int Response::_fork_and_exec(std::map<std::string, std::string>& meta_var, int* fd, int& pid, std::string path_to_root, std::string path_to_exec_prog, std::string url)
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
			char* args[] = {strdup(scriptPath), NULL};
			char** envp = _map_to_env(meta_var);
			execve(scriptPath.c_str(), args, envp);
			for (int i = 0; envp[i]; ++i)
				delete envp[i];

			delete envp;
			delete args[0];
		}
		else
		{
			char *args[] = {strdup(path_to_exec_prog), strdup(scriptPath), NULL};
			char **envp = _map_to_env(meta_var);
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

std::string Response::_get_cgi_output(int* fd)
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

int Response::_handle_cgi(const Request& request, std::string &rep, std::string url,
	std::string path_to_root, std::string path_to_exec_prog)
{
	std::map<std::string, std::string> meta_var = _generate_meta_variables(request, url);
	int fd[2];
	int pid = 0;

	if (access((path_to_root + url).c_str(), F_OK) == -1)
		return (404);
	if (path_to_exec_prog.empty() && access((path_to_root + url).c_str(), X_OK)
	== -1)
		return (403);

	if (_fork_and_exec(meta_var, fd, pid, path_to_root, path_to_exec_prog, url)
		== EXIT_FAILURE)
		return (EXIT_FAILURE);

	waitpid(pid, NULL, 0);

	rep = _get_cgi_output(fd);

	return (EXIT_SUCCESS);
}


char** Response::_map_to_env(std::map<std::string, std::string>& meta_var)
{
	char** env = new char*[meta_var.size() + 1];
	std::map<std::string, std::string>::iterator it;
	std::size_t i;

	for (it = meta_var.begin(), i = 0; it != meta_var.end(); ++i, ++it)
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

static void headerToVar(char& c)
{
	if (c == '-')
		c = '_';
	else
		c = std::toupper(c);
}


std::map<std::string, std::string> Response::_generate_meta_variables(const Request &request, std::string url)
{
  std::map<std::string, std::string> meta_var;

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::string tmp = it->first;
		std::for_each(tmp.begin(), tmp.end(), &headerToVar);
		meta_var["HTTP_" + tmp] = "[" + it->second + "]";
	}

	meta_var["REQUEST_METHOD"] = request.get_method();
	meta_var["REQUEST_URI"] = request.get_url();
	meta_var["SCRIPT_NAME"] = url;
	meta_var["QUERY_STRING"] = (request.get_url().find('?') == std::string::npos ? "" : request.get_url().substr(request.get_url().find('?') + 1));
	meta_var["AUTH_TYPE"] = "";      // Implement based on specific auth handling
	meta_var["CONTENT_LENGTH"] = get_headers_key("Content-Length");
	meta_var["CONTENT_TYPE"] = get_headers_key("Content-Type");
	meta_var["GATEWAY_INTERFACE"] = "CGI/1.1";
	if (url.size() < request.get_url().size())
		meta_var["PATH_INFO"] = request.get_url().substr(url.size(),
		std::string::npos);
	else
	 	meta_var["PATH_INFO"] = "";

	meta_var["PATH_TRANSLATED"] = ""; // Translate PATH_INFO to filesystem path
	meta_var["REMOTE_ADDR"] = "";     // To be retrieved from socket information
	meta_var["REMOTE_HOST"] = "";     // Optional: resolve REMOTE_ADDR to hostname
	meta_var["REMOTE_USER"] = "";     // User from authenticated session
	meta_var["SERVER_NAME"] = "";     // Retrieved from server configuration or host header
	meta_var["SERVER_PORT"] = "";     // Retrieved from server configuration
	meta_var["SERVER_PROTOCOL"] = "HTTP/1.1";
	meta_var["SERVER_SOFTWARE"] = "webserv-42/1.0";

  return meta_var;
}

bool Response::_is_cgi_request()
{
	if (_route == NULL || _route->cgi.empty())
		return (false);
	for (std::map<std::string, std::string>::const_iterator it = _route->cgi.begin(); it != _route->cgi.end(); ++it)
	{
		std::size_t i = _url.find(it->first);
		while (i != std::string::npos)
		{
			if (_is_a_file(_path_to_root + _url.substr(0, i + it->first.size())))
				break;
			i = _url.find(it->first, i + it->first.size());
		}
		if (i != std::string::npos)
		{
			std::string rep;
			std::string url = _url.substr(0, i + it->first.size());
			std::string path_info = _url.substr(i + it->first.size(),
				std::string::npos);
			_is_cgi = true;
			if (_handle_cgi(_request, rep, url, _path_to_root, it->second) != 0)
			{
				_generate_error(500);
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
