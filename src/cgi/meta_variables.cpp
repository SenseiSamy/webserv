/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   meta_variables.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 02:47:26 by snaji             #+#    #+#             */
/*   Updated: 2024/03/09 21:23:57 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>
#include <map>
#include <iostream>

char**	mapToEnv(std::map<std::string, std::string>& metaVar)
{
	char**	env = new char*[metaVar.size() + 1];
	std::map<std::string, std::string>::iterator	it;
	std::size_t i;
	
	for (it = metaVar.begin(), i = 0; it != metaVar.end(); ++i, ++it)
	{
		std::string	tmp = it->first + "=" + it->second;
		env[i] = new char[tmp.size() + 1];
		for (std::size_t j = 0; tmp.c_str()[j]; ++j)
			env[i][j] = tmp.c_str()[j];
		env[i][tmp.size()] = '\0';
	}
	env[i] = NULL;
	return (env);
}

static void	headerToVar(char& c)
{
	if (c == '-')
		c = '_';
	else
		c = std::toupper(c);
}

static void	tmpParseReqLine(std::string line, std::map<std::string, std::string>& metaVar)
{
	std::string	method;
	std::string	path;
	std::string	protocol;
	std::size_t	pos;

	pos = line.find(' ');
	method = line.substr(0, pos);
	line.erase(0, pos + 1);
	pos = line.find(' ');
	path = line.substr(0, pos);
	line.erase(0, pos + 1);
	protocol = line.substr(0, line.size() - 2);
	
	metaVar["REQUEST_METHOD"] = method;
	metaVar["SCRIPT_NAME"] = path.substr(0, path.find('?'));
	metaVar["QUERY_STRING"] = (path.find('?') == std::string::npos ? "" : path.substr(path.find('?') + 1));
}

static void	tmpParseReq(std::string request, std::map<std::string, std::string>& metaVar)
{
	std::string	line;
	std::string	tmp;
	std::size_t	pos;

	pos = request.find("\r\n");
	if (pos == std::string::npos)
		return;
	line = request.substr(0, pos);
	tmpParseReqLine(line, metaVar);
	request.erase(0, pos + 2);
	while ((pos = request.find("\r\n")) != std::string::npos && request != "\r\n")
	{
		line = request.substr(0, pos);
		tmp = line.substr(0, line.find(": "));
		std::for_each(tmp.begin(), tmp.end(), &headerToVar);
		metaVar["HTTP_" + tmp] = line.substr(line.find(": ") + 2, std::string::npos);
		request.erase(0, pos + 2);
	}
}

std::map<std::string, std::string>	generateMetaVariables(std::string const& request)
{
	std::map<std::string, std::string>	metaVar;

	tmpParseReq(request, metaVar);
	metaVar["AUTH_TYPE"] = ""; //TODO
	metaVar["CONTENT_LENGTH"] = ""; //TODO
	metaVar["CONTENT_TYPE="] = ""; //TODO
	metaVar["GATEWAY_INTERFACE"] = "CGI/1.1";
	metaVar["PATH_INFO"] = ""; //TODO
	metaVar["PATH_TRANSLATED"] = ""; //TODO
	metaVar["REMOTE_ADDR"] = ""; //TODO MANDATORY
	metaVar["REMOTE_HOST"] = ""; //TODO
	metaVar["REMOTE_USER"] = ""; //TODO
	metaVar["SERVER_NAME"] = ""; //TODO MANDATORY
	metaVar["SERVER_PORT"] = ""; //TODO MANDATORY
	metaVar["SERVER_PROTOCOL"] = "HTTP/1.1";
	metaVar["SERVER_SOFTWARE"] = "webserv-42/1.0";

	return (metaVar);
}