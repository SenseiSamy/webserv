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

const std::vector<std::string> read_files(const char path[])
{
    std::vector<std::string> files_content;
    std::ifstream file(path);

    if (!file.is_open() || !file.good())
    {
        std::cerr << "Error: could not open file " << path << std::endl;
        return files_content;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // delete comments
        std::string::size_type end = line.find('#');
        if (end != std::string::npos)
            line = line.substr(0, end);
        // delete leading and trailing whitespaces
        while (line.size() > 0 && std::isspace(line[0]))
            line.erase(0, 1);
        while (line.size() > 0 && std::isspace(line[line.size() - 1]))
            line.erase(line.size() - 1, 1);
        files_content.push_back(line);
    }

    file.close();
    if (files_content.size() == 0)
        std::cerr << "Warning: file " << path << " is empty" << std::endl;
    return files_content;
}

int syntax_keyword_routes(const std::vector<std::string> &files_content, std::size_t &line_number, std::size_t &column)
{

    return 0;
}

int syntax_keyword(const std::vector<std::string> &files_content)
{
    std::size_t line_number = 0;
    std::size_t column = 0;

    while (line_number < files_content.size())
    {
    }

    return 0;
}

const std::string get_word(const std::string &line, std::size_t &column)
{
    std::string word;
    while (column < line.size() && std::isspace(line[column]))
        ++column;
    while (column < line.size() && !std::isspace(line[column]))
        word += line[column++];
    return word;
}

int syntax_routes_keywords(const std::vector<std::string> &files_content, std::size_t &line_number, std::size_t &column)
{
    std::size_t i = line_number;
    std::size_t j = column;
    std::string word;

    return 0;
}

int syntax_server_keywords(const std::vector<std::string> &files_content, std::size_t &line_number, std::size_t &column)
{
    // take the first word
    
    if ()
}

int syntax_brackets(const std::vector<std::string> &files_content)
{
    std::size_t line_number = 0;
    ssize_t bracket_count = 0;

    for (std::size_t i = 0; i < files_content.size(); ++i)
    {
        for (std::string::size_type column = 0; column < files_content[i].size(); ++column)
        {
            if (files_content[i][column] == '{')
            {
                std::string directive;
                if (column != 0)
                {
                    std::string::size_type rev_j = column;
                    while (rev_j > 0 && std::isspace(files_content[i][rev_j - 1]))
                        --rev_j;
                    std::string::size_type start = rev_j;
                    while (rev_j > 0 && !std::isspace(files_content[i][rev_j - 1]))
                        --rev_j;
                    directive = files_content[i].substr(rev_j, start - rev_j);
                }
                else if (i > 0)
                {
                    std::string::size_type prev_line = i - 1;
                    while (prev_line > 0 && files_content[prev_line].size() == 0)
                        --prev_line;
                    if (files_content[prev_line].size() > 0)
                    {
                        std::string::size_type prev_line_size = files_content[prev_line].size();
                        while (prev_line_size > 0 && std::isspace(files_content[prev_line][prev_line_size - 1]))
                            --prev_line_size;
                        directive = files_content[prev_line].substr(0, prev_line_size);
                    }
                }
                if (directive != "routes"  && directive != "server")
                    return (std::cout << "Error: unknown directive at line " << line_number + 1 << std::endl), 1;
                ++bracket_count;
                if (directive == "server" && syntax_server_keywords(files_content, line_number, column) != 0)
                    return 1;
                else if (directive == "routes" && syntax_routes_keywords(files_content, line_number, column) != 0)
                    return 1;
            }
            else if (files_content[i][column] == '}')
                --bracket_count;
            if (bracket_count < 0)
                return (std::cout << "Error: too many closing brackets at line " << line_number + 1 << std::endl), 1;
        }
        ++line_number;
    }
    if (bracket_count > 0)
        return (std::cout << "Error: too many opening brackets at line " << line_number + 1 << std::endl), 1;

    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path to config file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::string> files_content = read_files(argv[1]);
    if (files_content.size() == 0)
        return EXIT_FAILURE;

    std::cout << "----------------------------------------" << std::endl;
    for (std::size_t i = 0; i < files_content.size(); ++i)
        std::cout << i + 1 << " >>> " << files_content[i] << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    if (syntax_brackets(files_content) != 0)
        std::cout << "\033[1;31mSyntax error\033[0m" << std::endl;
    else
        std::cout << "\033[1;32mSyntax is correct\033[0m" << std::endl;
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
