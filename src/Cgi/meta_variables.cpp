#include "Cgi.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <map>
#include <string>

char** Cgi::map_to_env(std::map<std::string, std::string>& meta_var)
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

static void tmpParseReqLine(std::string line, std::map<std::string, std::string>& meta_var)
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

static void tmpParseReq(std::string request, std::map<std::string, std::string>& meta_var)
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

void parseContentTypeAndLength(const std::string& originalRequest, std::map<std::string, std::string>& meta_var)
{
    std::string request = originalRequest;
    std::string::size_type pos = 0;
    std::string line, key, value;

    while ((pos = request.find("\r\n")) != std::string::npos)
    {
        line = request.substr(0, pos);
        size_t colonPos = line.find(": ");
        if (colonPos != std::string::npos)
        {
            key = line.substr(0, colonPos);
            value = line.substr(colonPos + 2);
            std::for_each(key.begin(), key.end(), headerToVar);
            if (key == "CONTENT_LENGTH" || key == "CONTENT_TYPE")
            {
                meta_var[key] = value;
            }
        }
        request.erase(0, pos + 2);
    }
}

std::map<std::string, std::string> Cgi::generate_meta_variables(std::string const& request)
{
    std::map<std::string, std::string> meta_var;

    tmpParseReq(request, meta_var);

    meta_var["AUTH_TYPE"] = "";      // Implement based on specific auth handling
    meta_var["CONTENT_LENGTH"] = ""; // Set in helper function
    meta_var["CONTENT_TYPE"] = "";   // Set in helper function
    meta_var["GATEWAY_INTERFACE"] = "CGI/1.1";
    meta_var["PATH_INFO"] = "";       // Specific handling based on URL parsing
    meta_var["PATH_TRANSLATED"] = ""; // Translate PATH_INFO to filesystem path
    meta_var["REMOTE_ADDR"] = "";     // To be retrieved from socket information
    meta_var["REMOTE_HOST"] = "";     // Optional: resolve REMOTE_ADDR to hostname
    meta_var["REMOTE_USER"] = "";     // User from authenticated session
    meta_var["SERVER_NAME"] = "";     // Retrieved from server configuration or host header
    meta_var["SERVER_PORT"] = "";     // Retrieved from server configuration
    meta_var["SERVER_PROTOCOL"] = "HTTP/1.1";
    meta_var["SERVER_SOFTWARE"] = "webserv-42/1.0";

    parseContentTypeAndLength(request, meta_var);

    return meta_var;
}
