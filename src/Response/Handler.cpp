#include "Response.hpp"

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


int Response::_get()
{
    _mime_type();

    std::string path = _path_root + _uri;
}

int Response::_post()
{
    return 0;
}

int Response::_delete()
{
    return 0;
}