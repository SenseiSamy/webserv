#ifndef PARSING_H
#define PARSING_H

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include <map>
#include <string>
#include <vector>

typedef struct Routes
{
    std::vector<std::string> methods;
    std::string redirect;
    std::string root;
    bool autoindex;
    std::string index;
    std::map<std::string, std::string> cgi;
    std::map<std::string, std::string> routes_pages;
} Routes;

typedef struct Server
{
    std::vector<std::string> host;
    unsigned short port;
    std::vector<std::string> server_names;
    std::map<std::string, std::string> error_pages;
    size_t body_size;
    std::vector<Routes> route;
} Server;

#endif // !PARSING_H

std::vector<std::string> split_line(const std::string &str)
{
    std::vector<std::string> words;
    std::string word;
    bool in_quote = false;

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '"' || str[i] == '\'')
            in_quote = !in_quote;

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
        {
            word += str[i];
        }
    }
    if (!word.empty())
        words.push_back(word);
    return words;
}

std::vector<std::vector<std::string> > read_files(const char path[])
{
    std::vector<std::vector<std::string> > files_content;
    std::ifstream file(path);
    if (!file.is_open() || !file.good())
    {
        std::cerr << "Error: could not open file " << path << std::endl;
        return files_content;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Remove comments
        std::string::size_type end = line.find('#');
        if (end != std::string::npos)
            line = line.substr(0, end);

        if (line.empty())
            continue;

        std::vector<std::string> parsed_line = split_line(line);
        if (parsed_line.empty())
            continue;

        files_content.push_back(parsed_line);
    }

    file.close();
    return files_content;
}

int syntax_brackets(const std::vector<std::vector<std::string> > &files_content)
{
    int open_brackets = 0;
    int close_brackets = 0;
    std::string last_word;

    for (size_t line = 0; line < files_content.size(); ++line)
    {
        for (size_t i = 0; i < files_content[line].size(); ++i)
        {
            // last word
            if (i > 0)
                last_word = files_content[line][i - 1];
            else if (line > 0)
                last_word = files_content[line - 1].back();
            std::string word = files_content[line][i];
            if (word == "{")
            {
                ++open_brackets;
                if (last_word != "server" && last_word != "route")
                {
                    std::cerr << "Syntax error: Unexpected '{' at line " << line + 1 << "." << std::endl;
                    return -1;
                }
            }
            else if (word == "}")
                ++close_brackets;
        }
    }

    if (open_brackets != close_brackets)
    {
        std::cerr << "Syntax error: Unbalanced brackets." << std::endl;
        return -1;
    }

    return 0;
}

int parsing_routes(const std::vector<std::string> &token_args, Routes &new_routes, const std::string &current_word,
                   size_t line)
{
    if (current_word == "methods")
    {
        for (size_t j = 0; j < token_args.size(); ++j)
        {
            if (token_args[j] != "GET" && token_args[j] != "POST" && token_args[j] != "DELETE")
                return (std::cerr << "Syntax error: Invalid method at line: " << line + 1 << "." << std::endl, -1);
        }
        new_routes.methods = token_args;
    }
    else if (current_word == "redirect")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid redirect at line: " << line + 1 << "." << std::endl, -1);

        new_routes.redirect = token_args[0];
    }
    else if (current_word == "root")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid root at line: " << line + 1 << "." << std::endl, -1);

        new_routes.root = token_args[0];
    }
    else if (current_word == "autoindex")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid autoindex at line: " << line + 1 << "." << std::endl, -1);
        if (token_args[0] != "on" && token_args[0] != "off")
            return (std::cerr << "Syntax error: Invalid autoindex value at line: " << line + 1 << "." << std::endl, -1);
        new_routes.autoindex = token_args[0] == "on";
    }
    else if (current_word == "default_index")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid index at line: " << line + 1 << "." << std::endl, -1);
        new_routes.index = token_args[0];
    }
    else if (current_word == "cgi")
    {
        if (token_args.size() != 2)
            return (std::cerr << "Syntax error: Invalid cgi at line: " << line + 1 << "." << std::endl, -1);
        new_routes.cgi[token_args[0]] = token_args[1];
    }
    else if (current_word == "routes_pages")
    {
        if (token_args.size() != 2)
            return (std::cerr << "Syntax error: Invalid routes_pages at line: " << line + 1 << "." << std::endl, -1);

        for (size_t j = 0; j < token_args[0].size(); ++j)
        {
            if (!std::isdigit(token_args[0][j]))
                return (std::cerr << "Syntax error: Invalid routes_pages value at line: " << line + 1 << "."
                                  << std::endl,
                        -1);
        }

        new_routes.routes_pages[token_args[0]] = token_args[1];
    }
    else
        return (std::cerr << "Syntax error: Invalid keyword at line: " << line + 1 << "." << std::endl, -1);
    return 0;
}

int parsing_server(const std::vector<std::string> &token_args, Server &new_server, const std::string &current_word,
                   size_t line)
{
    if (current_word == "port")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid port at line: " << line + 1 << "." << std::endl, -1);
        for (size_t j = 0; j < token_args[0].size(); ++j)
        {
            if (!std::isdigit(token_args[0][j]))
                return (std::cerr << "Syntax error: Invalid port value at line: " << line + 1 << "." << std::endl, -1);
        }
        std::istringstream iss(token_args[0]);
        iss >> new_server.port;
    }
    else if (current_word == "host")
    {
        if (token_args.size() < 1)
            return (std::cerr << "Syntax error: Invalid host at line: " << line + 1 << "." << std::endl, -1);

        new_server.host = token_args;
    }
    else if (current_word == "server_names")
    {
        if (token_args.size() < 1)
            return (std::cerr << "Syntax error: Invalid server_names at line: " << line + 1 << "." << std::endl, -1);

        new_server.server_names = token_args;
    }
    else if (current_word == "error_pages")
    {
        if (token_args.size() != 2)
            return (std::cerr << "Syntax error: Invalid error_pages at line: " << line + 1 << "." << std::endl, -1);

        new_server.error_pages[token_args[0]] = token_args[1];
    }
    else if (current_word == "body_size")
    {
        if (token_args.size() != 1)
            return (std::cerr << "Syntax error: Invalid body_size at line: " << line + 1 << "." << std::endl, -1);
        for (size_t j = 0; j < token_args[0].size(); ++j)
        {
            if (!std::isdigit(token_args[0][j]))
                return (std::cerr << "Syntax error: Invalid body_size value at line: " << line + 1 << "." << std::endl,
                        -1);
        }
        std::istringstream iss(token_args[0]);
        iss >> new_server.body_size;
    }
    else
        return (std::cerr << "Syntax error: Invalid keyword at line: " << line + 1 << "." << std::endl, -1);
    return 0;
}

int parsing_config(const std::vector<std::vector<std::string> > &files_content, std::vector<Server> &server)
{
    bool in_server = false;
    bool in_routes = false;

    Server new_server;
    Routes new_routes;
    for (size_t line = 0; line < files_content.size(); ++line)
    {
        for (size_t i = 0; i < files_content[line].size(); ++i)
        {
            std::string current_word = files_content[line][i];
            if (current_word == "server")
            {
                if (in_server)
                    return (std::cerr << "Syntax error: We are already in a server block at line: " << line + 1 << "."
                                      << std::endl,
                            -1);
                in_server = true;
                ++i;
                continue;
            }
            else if (current_word == "route")
            {
                if (!in_server)
                    return (std::cerr << "Syntax error: We are not in a server block at line: " << line + 1 << "."
                                      << std::endl,
                            -1);
                if (in_routes)
                    return (std::cerr << "Syntax error: We are already in a route block at line: " << line + 1 << "."
                                      << std::endl,
                            -1);
                in_routes = true;
                ++i;
                continue;
            }
            else if (current_word == "}")
            {
                if (in_routes)
                {
                    new_server.route.push_back(new_routes);
                    in_routes = false;
                }
                else if (in_server)
                {
                    server.push_back(new_server);
                    in_server = false;
                }
                continue;
            }
            else if (current_word == "{")
                continue;

            ++i;
            if (files_content[line][i] != "=")
                return (std::cerr << "Syntax error: Missing '=' at line: " << line + 1 << "." << std::endl, -1);
            ++i;
            std::vector<std::string> token_args;
            while (i < files_content[line].size())
            {
                if (files_content[line][i] == ";")
                    break;
                if (files_content[line][i][0] != '"' ||
                    files_content[line][i][files_content[line][i].size() - 1] != '"')
                    return (std::cerr << "Syntax error: Missing '\"' at line: " << line + 1 << "." << std::endl, -1);
                // Remove quotes
                token_args.push_back(files_content[line][i].substr(1, files_content[line][i].size() - 2));
                ++i;
            }
            if (token_args.empty())
                return (std::cerr << "Syntax error: Missing arguments at line: " << line + 1 << "." << std::endl, -1);
            if (i < files_content[line].size() - 1 && files_content[line][i + 1] != ";")
                return (std::cerr << "Syntax error: Missing ';' at line: " << line + 1 << "." << std::endl, -1);

            if (in_routes && token_args.size() >= 1)
            {
                if (parsing_routes(token_args, new_routes, current_word, line) == -1)
                    return -1;
            }
            else if (in_server && token_args.size() >= 1)
            {
                if (parsing_server(token_args, new_server, current_word, line) == -1)
                    return -1;
            }
            else
                return (std::cerr << "Syntax error: Invalid keyword at line: " << line + 1 << "." << std::endl, -1);
        }
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path to config file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::vector<std::string> > files_content = read_files(argv[1]);
    if (files_content.empty())
        return EXIT_FAILURE;

    // for (size_t i = 0; i < files_content.size(); ++i)
    // {
    //     std::cout << ">>> {";
    //     for (size_t j = 0; j < files_content[i].size(); ++j)
    //         std::cout << "\"" << files_content[i][j] << "\", ";
    //     std::cout << "}" << std::endl;
    // }

    int result = syntax_brackets(files_content);
    // if (result == 0)
    //     std::cout << "Syntax is correct." << std::endl;
    // else
    //     std::cout << "Syntax error detected." << std::endl;

    if (result == 0)
    {
        std::vector<Server> server;

        result = parsing_config(files_content, server);
        if (result == 0)
            std::cout << "Keywords syntax is correct." << std::endl;
        else
        {
            std::cout << "Keywords syntax error detected." << std::endl;
            return EXIT_FAILURE;
        }


        // display server content
        std::cout << "----------------------------------------" << std::endl;
        for (size_t i = 0; i < server.size(); ++i)
        {
            std::cout << "Server " << i + 1 << ":" << std::endl;
            std::cout << "  port: " << server[i].port << std::endl;
            std::cout << "  host: ";
            for (size_t j = 0; j < server[i].host.size(); ++j)
                std::cout << server[i].host[j] << " ";
            std::cout << std::endl;
            std::cout << "  server_names: ";
            for (size_t j = 0; j < server[i].server_names.size(); ++j)
                std::cout << server[i].server_names[j] << " ";
            std::cout << std::endl;
            std::cout << "  error_pages: ";
            for (std::map<std::string, std::string>::iterator it = server[i].error_pages.begin();
                 it != server[i].error_pages.end(); ++it)
                std::cout << it->first << " " << it->second << " ";
            std::cout << std::endl;
            std::cout << "  body_size: " << server[i].body_size << std::endl;

            for (size_t j = 0; j < server[i].route.size(); ++j)
            {
                std::cout << "  Routes " << j + 1 << ":" << std::endl;
                std::cout << "    methods: ";
                for (size_t k = 0; k < server[i].route[j].methods.size(); ++k)
                    std::cout << server[i].route[j].methods[k] << " ";
                std::cout << std::endl;
                std::cout << "    redirect: " << server[i].route[j].redirect << std::endl;
                std::cout << "    root: " << server[i].route[j].root << std::endl;
                std::cout << "    autoindex: " << server[i].route[j].autoindex << std::endl;
                std::cout << "    default_index: " << server[i].route[j].index << std::endl;
                std::cout << "    cgi: ";
                for (std::map<std::string, std::string>::iterator it = server[i].route[j].cgi.begin();
                     it != server[i].route[j].cgi.end(); ++it)
                    std::cout << it->first << " " << it->second << " ";
                std::cout << std::endl;
                std::cout << "    routes_pages: ";
                for (std::map<std::string, std::string>::iterator it = server[i].route[j].routes_pages.begin();
                     it != server[i].route[j].routes_pages.end(); ++it)
                    std::cout << it->first << " " << it->second << " ";
                std::cout << std::endl;
            }
            std::cout << "----------------------------------------" << std::endl;
        }
    }
    return EXIT_SUCCESS;
}

/*
DOC:

g++ -Wall -Wextra -Werror -O0 -std=c++98 -ggdb3 parsing.cpp -o parsing

./parsing example.conf

TASK:

Check brackets syntax ✅
Check the keyword value syntax
*/
