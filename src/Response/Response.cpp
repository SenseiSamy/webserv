#include "Response.hpp"

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

Response::Response(const Request& req, const server_data& server)
    : _req(req), _server(server), _http_version("HTTP/1.1"), _status_code(200)
{
}

Response::~Response()
{
}

std::string Response::get_status_message(int status_code)
{
    switch (status_code)
    {
        case 200:
            return "OK";
        case 404:
            return "Not Found";
        case 500:
            return "Internal Server Error";
        default:
            return "Unknown Status";
    }
}

std::string Response::serialize() const
{
    std::ostringstream response_stream;
    response_stream << _http_version << " " << _status_code << " " << get_status_message(_status_code) << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        response_stream << it->first << ": " << it->second << "\r\n";
    response_stream << "\r\n" << _body;
    return response_stream.str();
}

std::string Response::get_mime_type(const std::string& filename)
{
    static std::map<std::string, std::string> mime_types;
    mime_types[".html"] = "text/html";
    mime_types[".txt"] = "text/plain";
    mime_types[".jpg"] = "image/jpeg";
    mime_types[".png"] = "image/png";
    mime_types[".gif"] = "image/gif";
    mime_types[".css"] = "text/css";
    mime_types[".js"] = "application/javascript";

    size_t dot_pos = filename.rfind('.');
    if (dot_pos != std::string::npos)
    {
        std::string ext = filename.substr(dot_pos);
        if (mime_types.count(ext))
            return mime_types[ext];
    }
    return "application/octet-stream"; // Default MIME type
}

void Response::handle_get()
{
    std::string resource_path = _server.root + _req.get_uri();
    std::ifstream file(resource_path.c_str(), std::ios::binary);
    if (!file)
    {
        std::cerr << "File not found: " << resource_path << std::endl;
        _body = "Error 404: Not Found\n";
        set_header("Content-Type", "text/plain");
        _status_code = 404; // Not Found
    }
    else
    {
        std::ostringstream ss;
        ss << file.rdbuf();
        _body = ss.str();
        file.close();

        set_header("Content-Type", get_mime_type(resource_path));
        _status_code = 200; // OK
    }
}
/* -------------- POST -------------- */

std::string Response::get_boundary(const std::string& header)
{
    size_t pos = header.find("boundary=");
    if (pos != std::string::npos)
        return header.substr(pos + 9); // Length of 'boundary=' is 9
    return "";
}

bool Response::parse_multipart_form_data(const std::string& data, const std::string& boundary,
                                         std::vector<std::string>& files)
{
    size_t pos = 0, start;
    std::string delimiter = "--" + boundary + "\r\n";
    std::string end_delimiter = "--" + boundary + "--";
    while ((start = data.find(delimiter, pos)) != std::string::npos)
    {
        start += delimiter.length();
        size_t end = data.find("\r\n", start);
        if (end == std::string::npos)
            return false;

        std::string content_disposition = data.substr(start, end - start);
        std::string filename_key = "filename=\"";
        size_t filename_pos = content_disposition.find(filename_key);
        if (filename_pos != std::string::npos)
        {
            filename_pos += filename_key.length();
            size_t filename_end = content_disposition.find("\"", filename_pos);
            std::string filename = content_disposition.substr(filename_pos, filename_end - filename_pos);
            files.push_back(filename);

            // Find start of file data
            pos = data.find("\r\n\r\n", end) + 4; // Skip two CRLFs
            end = data.find(delimiter, pos) - 2;  // Stop at the CRLF before the next part
            if (end == std::string::npos || end <= pos)
                return false;

            // Save file data to /tmp
            std::string full_path = "/tmp/" + filename;
            std::ofstream file(full_path.c_str(), std::ios::binary);
            if (!file.write(data.c_str() + pos, end - pos))
            {
                file.close();
                return false;
            }
            file.close();
        }
        pos = end + delimiter.length();
    }
    return true;
}

void Response::handle_post()
{
    std::string content_type = _req.get_header_value("Content-Type");
    std::string boundary = get_boundary(content_type);
    const std::string& data = _req.get_body();

    if (content_type.find("multipart/form-data") != std::string::npos && !boundary.empty())
    {
        std::vector<std::string> uploaded_files;
        if (!parse_multipart_form_data(data, boundary, uploaded_files))
        {
            _body = "Error processing uploaded files";
            _status_code = 500; // Internal Server Error
        }
        else
        {
            _body = "Files uploaded successfully: ";
            for (size_t i = 0; i < uploaded_files.size(); i++)
                _body += uploaded_files[i] + " ";
            _status_code = 200; // OK
        }
    }
    else
    {
        _body = "Unsupported Media Type or incorrect data";
        _status_code = 415; // Unsupported Media Type
    }
    set_header("Content-Type", "text/plain");
}

/* -------------- DELETE -------------- */

void Response::handle_delete()
{
    std::string resource_path = "/tmp" + _req.get_uri(); // Assuming files to delete are in /tmp

    // Check if the file exists and can be accessed
    std::ifstream file(resource_path.c_str());
    if (!file)
    {
        _body = "Resource not found: " + resource_path;
        _status_code = 404; // Not Found
        set_header("Content-Type", "text/plain");
        return;
    }
    file.close(); // Close the file before attempting to delete it

    // Attempt to delete the file
    if (std::remove(resource_path.c_str()) == 0)
    {
        _body = "Resource " + resource_path + " deleted successfully";
        _status_code = 200; // OK
    }
    else
    {
        // Check errno for the specific error
        switch (errno)
        {
            case EACCES:
                _body = "Permission denied to delete resource: " + resource_path;
                _status_code = 403; // Forbidden
                break;
            case ENOENT:
                _body = "Resource not found: " + resource_path;
                _status_code = 404; // Not Found
                break;
            default:
                _body = "Error deleting resource: " + resource_path;
                _status_code = 500; // Internal Server Error
        }
    }
    set_header("Content-Type", "text/plain");
}