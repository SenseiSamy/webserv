#include "Response.hpp"
#include <fstream>
#include <sstream>

void Response::_http_error(unsigned short status_code)
{
    for (std::map<unsigned short, std::string>::iterator it = _error_map.begin(); it != _error_map.end(); ++it)
    {
        if (it->first == _status_code)
        {
            _uri = "/" + it->second;
            _get();
            return;
        }
    }

    std::ifstream file("www/ErrorPage");
    std::string line;
    _status_code = status_code;
    _status_message = _error_map[status_code];
    while (std::getline(file, line))
        _body += line + "\n";
    file.close();

    std::stringstream ss;
    ss << _status_code;

    _body += "<img src=\"https://httpgoats.com/";
    _body += ss.str() + ".jpg\" alt=\"Centered Image\"\n";
    _body += "width=\"800\"\nheight=\"600\"\n/>";
    _body += "</div>\n</body>\n</html>\n\r";
}

void Response::_mime_type()
{
    std::string extension = _uri.substr(_uri.find_last_of(".") + 1);

    if (extension == "html" || extension == "htm")
        _header["Content-Type"] = "text/html";
    else if (extension == "jpg" || extension == "jpeg")
        _header["Content-Type"] = "image/jpeg";
    else if (extension == "png")
        _header["Content-Type"] = "image/png";
    else if (extension == "gif")
        _header["Content-Type"] = "image/gif";
    else if (extension == "css")
        _header["Content-Type"] = "text/css";
    else if (extension == "ico")
        _header["Content-Type"] = "image/x-icon";
    else
        _header["Content-Type"] = "text/plain";
}

int Response::_post()
{
    return 0;
}

int Response::_delete()
{
    return 0;
}