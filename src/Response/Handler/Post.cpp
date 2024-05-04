#include "Response.hpp"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>

int Response::_post()
{
    std::string boundary = _request.get_header("Content-Type");
    std::string::size_type pos = boundary.find("boundary=");
    if (pos == std::string::npos)
        return 0;
    
    std::istringstream file(_request.get_body());
    std::string file_name, line;
    size_t body_read = 0;
    int token = 2;
    while (std::getline(file, line))
    {
        if (line.find(boundary) != std::string::npos)
            body_read += line.length();
        if (line.find(boundary) != std::string::npos)
        {
            file_name = line.substr(line.find("filename=") + 10);
            token--;
            body_read += line.length();
        }
        else if (line.find("Content-Type") != std::string::npos)
        {
            token--;
            body_read += line.length();
        }
        if (token == 0)
            break;
    }
    file_name = file_name.substr(0, file_name.size() - 2); // Remove the last two characters
    std::getline(file, line);
    std::ofstream new_file(file_name.c_str(), std::ios::binary);
    char buff[1024];
    size_t content = 1;
    while (1024 * content < _request.get_content_length())
    {
        file.read(buff, 1024);
        new_file.write(buff, 1024);
        content++;
    }
    file.read(buff, _request.get_content_length() - 1024 * (content - 1) + (body_read + boundary.length() + 10));
    new_file.write(buff, _request.get_content_length() - 1024 * (content - 1) + (body_read + boundary.length() + 10));
    new_file.close();
    return 0;
}