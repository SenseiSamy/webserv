#include "Cgi.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <map>
#include <string>

char **Cgi::map_to_env(std::map<std::string, std::string> &meta_var)
{
    char **env = new char *[meta_var.size() + 1];
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

static void headerToVar(char &c)
{
    if (c == '-')
        c = '_';
    else
        c = std::toupper(c);
}

static void tmpParseReqLine(std::string line, std::map<std::string, std::string> &meta_var)
{
    std::string method;
    std::string path;
    std::string protocol;
    std::size_t pos;

    pos = line.find(' ');
    method = line.substr(0, pos);
    line.erase(0, pos + 1);
    pos = line.find(' ');
    path = line.substr(0, pos);
    line.erase(0, pos + 1);
    protocol = line.substr(0, line.size() - 2);

    meta_var["REQUEST_METHOD"] = method;
    meta_var["SCRIPT_NAME"] = path.substr(0, path.find('?'));
    meta_var["QUERY_STRING"] = (path.find('?') == std::string::npos ? "" : path.substr(path.find('?') + 1));
}

static void tmpParseReq(std::string request, std::map<std::string, std::string> &meta_var)
{
    std::string line;
    std::string tmp;
    std::size_t pos;

    pos = request.find("\r\n");
    if (pos == std::string::npos)
        return;
    line = request.substr(0, pos);
    tmpParseReqLine(line, meta_var);
    request.erase(0, pos + 2);
    while ((pos = request.find("\r\n")) != std::string::npos && request != "\r\n")
    {
        line = request.substr(0, pos);
        tmp = line.substr(0, line.find(": "));
        std::for_each(tmp.begin(), tmp.end(), &headerToVar);
        meta_var["HTTP_" + tmp] = line.substr(line.find(": ") + 2, std::string::npos);
        request.erase(0, pos + 2);
    }
}

std::map<std::string, std::string> Cgi::generate_meta_variables(std::string const &request)
{
    std::map<std::string, std::string> meta_var;

    tmpParseReq(request, meta_var);
    meta_var["AUTH_TYPE"] = "";      // TODO
    meta_var["CONTENT_LENGTH"] = ""; // TODO
    meta_var["CONTENT_TYPE="] = "";  // TODO
    meta_var["GATEWAY_INTERFACE"] = "CGI/1.1";
    meta_var["PATH_INFO"] = "";       // TODO
    meta_var["PATH_TRANSLATED"] = ""; // TODO
    meta_var["REMOTE_ADDR"] = "";     // TODO MANDATORY
    meta_var["REMOTE_HOST"] = "";     // TODO
    meta_var["REMOTE_USER"] = "";     // TODO
    meta_var["SERVER_NAME"] = "";     // TODO MANDATORY
    meta_var["SERVER_PORT"] = "";     // TODO MANDATORY
    meta_var["SERVER_PROTOCOL"] = "HTTP/1.1";
    meta_var["SERVER_SOFTWARE"] = "webserv-42/1.0";

    return (meta_var);
}