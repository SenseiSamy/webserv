#include "Response.hpp"

#include <cstddef>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>

static int is_directory(const std::string &path)
{
    struct stat s;
    return stat(path.c_str(), &s) == 0 && s.st_mode & S_IFDIR;
}

bool Response::_is_cgi_request()
{
    if (!_route || _route->cgi.empty())
        return false;

    std::string::size_type found = _uri.find_last_of(".");
    if (found == std::string::npos)
        return false;

    std::string extension = _uri.substr(found);
    std::map<std::string, std::string>::iterator it = _route->cgi.find(extension);
    if (it == _route->cgi.end())
        return false;

    std::string rep;
    if (_cgi(rep, _uri, _server.root, it->second))
        _http_error(500);

    _is_cgi = true;
    _status_code = 200;
    _status_message = _error_map[200];
    _body = rep;
    _set_content_length();
    return true;
}

void Response::_directory_listing()
{
    struct dirent *entry;
    DIR *dp = opendir((_path_root + _uri).c_str());
    if (dp == NULL)
    {
        _http_error(404);
        return;
    }

    _status_code = 200;
    _status_message = _error_map[200];
    _header["Content-Type"] = "text/html";

    _body += "<ul>\n";
    _body += "<li><a href=..>..</a></li?>";
    while ((entry = readdir(dp)))
    {
        if (std::string(entry->d_name) == ".." || std::string(entry->d_name) == ".")
            continue;
        _body += "<li><a href=\"";
        _body += _uri + "/" + entry->d_name;
        _body += "\">";
        _body += entry->d_name;
        _body += "</a></li>\n";
    }

    _set_content_length();
}

void Response::_redirect()
{
    if (!_route || _route->file_path.empty())
    {
        _http_error(500);
        return;
    }

    _status_code = 307;
    _status_message = _error_map[307];
    _header["Location"] = _route->file_path;
}

int Response::_check_file_uri()
{
    std::string path = _path_root + _uri;
    if (access(path.c_str(), F_OK) == -1)
        return 404;
    if (access(path.c_str(), R_OK) == -1)
        return 403;

    char buff[PATH_MAX];
    if (realpath(_uri.c_str(), buff) == NULL)
        return 500;
    _uri = buff;
    if (getcwd(buff, PATH_MAX) == NULL)
        return 500;
    if (_uri.find(buff) != 0)
        return 403;
    _uri.erase(0, std::string(buff).size());
    return 0;
}

int Response::_get()
{
    if (_route && _route->path.empty())
    {
        _redirect();
        return 0;
    }

    if (_request.get_uri() == "/")
        _uri = _server.root + "/index.html";
    else
        _uri = _server.root + _request.get_uri();

    if (_route && is_directory(_uri))
    {
        if (_route->path.empty())
            _uri += "/" + _route->path;
        else
        {
            if (_route->directory_listing)
                _directory_listing();
            else
                _http_error(404);
            return 0;
        }
    }

    _mime_type();
    int error = _check_file_uri();
    if (error != 0)
    {
        _http_error(error);
        return 0;
    }

    if (_is_cgi_request())
        return 0;
    std::ifstream file((_path_root + _uri).c_str());
    if (!file.is_open())
    {
        _http_error(500);
        return 0;
    }

    _status_code = 200;
    _status_message = _error_map[200];
    _set_content_length();
    return 0;
}
