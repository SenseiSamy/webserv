#ifndef PARSING_H
#define PARSING_H

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>

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
} Routes;

typedef struct Server
{
    std::string host;
    unsigned short port;
    std::vector<std::string> server_names;
    std::map<std::string, std::string> error_pages;
    size_t body_size;
    std::vector<Routes> routes;
} Server;

#endif // !PARSING_H

int syntax_brackets(std::vector<std::string> &files_content)
{

    for (std::size_t i = 0; i < files_content.size(); ++i)
    {
        std::string::size_type end = files_content[i].find('#');
        if (end == std::string::npos)
            end = files_content[i].size();
        files_content[i] = files_content[i].substr(0, end);
    }

    bool in_server = false;
    bool in_routes = false;

    std::string line;
    for (std::size_t line_nb = 0; line_nb < files_content.size(); ++line_nb)
    {
        line = files_content[line_nb];
        // display the line but with every { and } replaced in red
        std::string display_line;
        for (std::string::size_type column = 0; column < line.size(); ++column)
        {
            if (line[column] == '{')
                display_line += "\033[31m{\033[0m";
            else if (line[column] == '}')
                display_line += "\033[31m}\033[0m";
            else
                display_line += line[column];
        }
        std::cout << ">>> " << display_line << std::endl;

        for (std::string::size_type column = 0; column < line.size(); ++column)
        {
            if (std::isspace(line[column]))
                continue;
            // check if the word was "server"
            if (line.substr(column, 6) == "server" && (line[column + 6] == ' ' || line[column + 6] == '{'))
            {
                if (in_server)
                    return (std::cerr << "Error: server block already opened at line " << line_nb << std::endl,
                            EXIT_FAILURE);
                in_server = true;
                column += 6;
                while (column < line.size() && std::isspace(line[column]))
                    ++column;
                if (column == line.size())
                {
                    for (std::size_t j = line_nb + 1; j < files_content.size(); ++j)
                    {
                        if (files_content[j].empty())
                            continue;
                        if (files_content[j][0] == '{')
                            break;
                        return (std::cerr << "Error: missing '{' after server at line " << line_nb << std::endl,
                                EXIT_FAILURE);
                    }
                }
            }
            // check if the word was "routes"
            else if (line.substr(column, 6) == "routes" && (line[column + 6] == ' ' || line[column + 6] == '{'))
            {
                if (!in_server)
                    return (std::cerr << "Error: routes block outside of server block at line " << line_nb << std::endl,
                            EXIT_FAILURE);
                if (in_routes)
                    return (std::cerr << "Error: routes block already opened at line " << line_nb << std::endl,
                            EXIT_FAILURE);

                in_routes = true;
                column += 6;
                while (column < line.size() && std::isspace(line[column]))
                    ++column;
                if (column == line.size() || line[column] != '{')
                    return (std::cerr << "Error: missing '{' after routes at line " << line_nb << std::endl,
                            EXIT_FAILURE);
            }
            // check if the word was "}"
            else if (line[column] == '}')
            {
                if (in_routes)
                {
                    in_routes = false;
                    continue;
                }
                if (in_server)
                {
                    in_server = false;
                    continue;
                }
                return (std::cerr << "Error: '}' outside of server or routes block at line " << line_nb << std::endl,
                        EXIT_FAILURE);
            }
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path to config file>" << std::endl;
        return EXIT_FAILURE;
    }

    // try to open the file
    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::string> files_content;

    // read the file
    std::string line;
    while (std::getline(file, line))
        files_content.push_back(line);

    // display the check_syntax result
    if (syntax_brackets(files_content) == EXIT_SUCCESS)
        std::cout << "Brackets syntax is correct" << std::endl;
    else
        std::cout << "Brackets syntax is incorrect" << std::endl;
    // parse the file
    // std::vector<Tokens> tokens = tokenise(files_content);

    // print the tokens
    // std::cout << "--------------------------------------------" << std::endl;
    // for (size_t i = 0; i < tokens.size(); ++i)
    // {
    //     std::cout << "Token: {" << tokens[i].token << "}\nLine: " << tokens[i].line << "\tColumn: " <<
    //     tokens[i].column
    //               << "\tIs string: " << tokens[i].is_string << std::endl;
    //     std::cout << "--------------------------------------------" << std::endl;
    // }

    return EXIT_SUCCESS;
}

/*
DOC:

g++ -Wall -Wextra -Werror -O0 -std=c++98 -ggdb3 parsing.cpp -o parsing

./parsing example.conf

TASK:

Check brackets syntax ✅
Check
*/
