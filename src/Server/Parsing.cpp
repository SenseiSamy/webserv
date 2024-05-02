#include "Server.hpp"

#include <cstddef>
#include <fstream>
#include <sstream>

static const std::vector<std::string> split_line(const std::string &str)
{
    std::vector<std::string> words;
    std::string word;
    bool in_quote = false;

    for (std::string::size_type i = 0; i < str.size(); ++i)
    {
        if (str[i] == '"' || str[i] == '\'')
            in_quote = !in_quote;

        if (!in_quote && str[i] == '#')
            break;

        if (in_quote)
        {
            word += str[i];
            continue;
        }

        if (std::isspace(str[i]) || str[i] == '=' || str[i] == ';' || str[i] == '{' || str[i] == '}')
        {
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
            if (!std::isspace(str[i]))
                words.push_back(std::string(1, str[i]));
        }
        else
            word += str[i];
    }
    if (!word.empty())
        words.push_back(word);
    return words;
}

void Server::read_config()
{
    std::ifstream file(_config_file.c_str());
    if (!file.is_open())
        throw std::runtime_error("Failed to open file : " + std::string(strerror(errno)));

    size_t line_number = 0;
    std::string line;
    while (std::getline(file, line))
    {
        ++line_number;
        std::vector<std::string> words = split_line(line);
        if (words.empty())
            _content_file[line_number] = std::vector<std::string>();
        else
            for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it)
                _content_file[line_number].push_back(*it);
    }

    file.close();
}

static ssize_t to_size_t(std::string value)
{
    ssize_t result;
    std::istringstream(value) >> result;
    return result;
}

const std::vector<std::string> Server::get_value(const std::string &token)
{
    std::vector<std::string> result;

    // check if the next word is "="
    if (next_word() != "=")
        throw std::runtime_error("Syntax error: Expected '=' after " + token + " at line " + to_string(_current_line));

    // get the value
    std::string word = next_word();
    while (word != "" && word != ";")
    {
        if ((word[0] == '"' && word[word.size() - 1] == '"') || (word[0] == '\'' && word[word.size() - 1] == '\''))
            result.push_back(word.substr(1, word.size() - 2));
        else
        {
            if (word != ";")
                throw std::runtime_error("Syntax error: Expected ';' after value at line " + to_string(_current_line));
            else
                throw std::runtime_error("Syntax error: Missing \" or ' at line " + to_string(_current_line));
        }
        word = next_word();
    }

    if (result.empty())
        throw std::runtime_error("Syntax error: Expected value after " + token + " at line " +
                                 to_string(_current_line));

    if (word != ";")
        throw std::runtime_error("Syntax error: Expected ';' after value at line " + to_string(_current_line));

    return result;
}

#include <iostream>

const route Server::parse_route()
{
    route result;

    std::string word = next_word();
    while (word != "" && word != "}")
    {
        std::vector<std::string> value = get_value(word);
        std::cout << "word: " << word << std::endl;
        for (std::vector<std::string>::iterator it = value.begin(); it != value.end(); ++it)
            std::cout << "value: " << *it << std::endl;

        if (word == "cgi" && value.size() != 2)
            throw std::runtime_error("Syntax error: Expected two values after " + word + " at line " +
                                     to_string(_current_line));
        else if (word != "cgi" && value.size() != 1)
            throw std::runtime_error("Syntax error: Expected only one value after " + word + " at line " +
                                     to_string(_current_line));

        if (word == "path")
            result.path = value[0];
        else if (word == "http_methods")
            result.accepted_methods = value;
        else if (word == "root")
            result.root = value[0];
        else if (word == "file_path")
            result.file_path = value[0];
        else if (word == "directory_listing")
            result.directory_listing = value[0] == "true";
        else if (word == "default_file")
            result.default_file = value[0];
        else if (word == "cgi")
            result.cgi[value[0]] = value[1];
        else if (word == "cgi_upload_path")
            result.cgi_upload_path = value[0];
        else if (word == "cgi_upload_enable")
            result.cgi_upload_enable = value[0] == "true";
        else
            throw std::runtime_error("Syntax error: Invalid keyword " + word + " at line " + to_string(_current_line));

        word = next_word();
    }

    if (word != "}")
        throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(_current_line));

    if (result.path.empty())
        throw std::runtime_error("Syntax error: Missing path at line " + to_string(_current_line));

    return result;
}

const server Server::parse_server()
{
    server result;

    result.host = "";
    result.port = 0;
    result.root = "";
    result.server_names = "";
    result.default_server = false;
    result.max_body_size = 0;
    result.error_pages = std::map<std::string, std::string>();
    result.routes = std::vector<route>();

    std::string word = next_word();
    while (word != "" && word != "}")
    {
        if (word == "route")
        {
            std::string next = next_word();
            if (next != "{")
                throw std::runtime_error("Syntax error: Expected '{' after route at line " + to_string(_current_line));
            result.routes.push_back(parse_route());
        }
        else
        {
            std::vector<std::string> value = get_value(word);

            if (word != "error_pages" && value.size() != 1)
                throw std::runtime_error("Syntax error: Expected only one value after " + word + " at line " +
                                         to_string(_current_line));
            else if (word == "error_pages" && value.size() != 2)
                throw std::runtime_error("Syntax error: Expected two values after " + word + " at line " +
                                         to_string(_current_line));

            if (word == "host")
                result.host = value[0];
            else if (word == "port")
                result.port = to_size_t(value[0]);
            else if (word == "root")
                result.root = value[0];
            else if (word == "server_names")
                result.server_names = value[0];
            else if (word == "default_server")
            {
                if (value[0] == "true")
                    result.default_server = true;
                else if (value[0] == "false")
                    result.default_server = false;
                else
                    throw std::runtime_error("Syntax error: Invalid value for default_server at line " +
                                             to_string(_current_line));
            }
            else if (word == "max_body_size")
                result.max_body_size = to_size_t(value[0]);
            else if (word == "error_pages")
                result.error_pages[value[0]] = value[1];
            else
                throw std::runtime_error("Syntax error: Invalid keyword " + word + " at line " +
                                         to_string(_current_line));
        }
        word = next_word();
    }

    if (word != "}")
        throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(_current_line));

    // check if the host is a valid IPv4 address
    std::vector<std::string> parts;
    std::string host = result.host;

    while (!host.empty())
    {
        size_t pos = host.find('.');
        if (pos == std::string::npos)
        {
            parts.push_back(host);
            break;
        }
        parts.push_back(host.substr(0, pos));
        host.erase(0, pos + 1);
    }
    if (parts.size() != 4)
        throw std::runtime_error("Syntax error: Invalid host address at line " + to_string(_current_line));
    for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
    {
        if (it->empty())
            throw std::runtime_error("Syntax error: Invalid host address at line " + to_string(_current_line));
        for (std::string::iterator it2 = it->begin(); it2 != it->end(); ++it2)
        {
            if (!std::isdigit(*it2))
                throw std::runtime_error("Syntax error: Invalid host address at line " + to_string(_current_line));
        }
        if (to_size_t(*it) < 0 || to_size_t(*it) > 255)
            throw std::runtime_error("Syntax error: Invalid host address at line " + to_string(_current_line));
    }

    if (result.port < 1024 || result.port > 49151)
        throw std::runtime_error("Syntax error: Invalid port number at line " + to_string(_current_line));

    return result;
}

void Server::parsing_config()
{
    std::string word = next_word();

    while (true)
    {
        if (word == "server")
        {
            std::string next = next_word();
            if (next != "{")
                throw std::runtime_error("Syntax error: Expected '{' after server at line " + to_string(_current_line));
            _servers.push_back(parse_server());
        }
        else
            throw std::runtime_error("Syntax error: Invalid keyword " + word + " at line " + to_string(_current_line));
        word = next_word();
        if (word == "")
            break;
    }
}
