#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#define COMMENT "#"
#define EQUAL ":"
#define DELIMITER ","
#define OPEN_BRACE "{"
#define CLOSE_BRACE "}"
#define END_OF_LINE ";"

#define SERVER "server"
#define ROUTE "route"

#define PORT "port"
#define HOST "host"
#define ERROR_PAGES "error_pages"
#define CLIENT_MAX_BODY_SIZE "client_max_body_size"

#define PATH "path"
#define ALLOW_METHODS "allow_methods"
#define REDIRECT_TO "redirect_to"
#define ROOT "root"
#define DIRECTORY_LISTING "directory_listing"
#define DEFAULT_FILE "default_file"
#define CGI "cgi"
#define UPLOAD_PATH "upload_path"

std::string to_string(const size_t &line)
{
    std::stringstream ss;
    ss << line;
    return ss.str();
}

size_t to_integer(const std::string &str)
{
    size_t number;
    std::stringstream ss(str);
    ss >> number;
    return number;
}

std::vector<std::string> split(std::ifstream &file)
{
    std::vector<std::string> words;
    std::string line;

    while (std::getline(file, line))
    {
        std::string word;
        for (size_t i = 0; i < line.size(); i++)
        {
            if (line[i] == COMMENT[0])
                break;
            if (line[i] == EQUAL[0] || line[i] == DELIMITER[0] || line[i] == OPEN_BRACE[0] || line[i] == CLOSE_BRACE[0] || line[i] == END_OF_LINE[0])
            {
                if (!word.empty())
                {
                    words.push_back(word);
                    word.clear();
                }
                words.push_back(std::string(1, line[i]));
            }
            else if (std::isspace(line[i]))
            {
                if (!word.empty())
                {
                    words.push_back(word);
                    word.clear();
                }
            }
            else
                word += line[i];
        }
        if (!word.empty())
            words.push_back(word);
    }
    return words;
}

void checkSyntax(const std::vector<std::string> &words, std::stack<std::string> &braces, size_t &i, size_t &line)
{
    if (words.empty())
        return;

    if (!braces.empty() && braces.top() == SERVER)
    {
        if (words[i] == PORT)
        {
            if (i + 1 < words.size() && words[i + 1] == EQUAL)
            {
                i++;
                if (i + 1 >= words.size())
                    throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected a number after ':'.");
                for (size_t j = 0; j < words[i + 1].size() && words[i + 1][j] != END_OF_LINE[0]; j++)
                {
                    if (!std::isdigit(words[i + 1][j]))
                        throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected a number after ':'.");
                }
                int port = to_integer(words[i + 1]);
                if (port == 25565)
                    throw std::runtime_error("Syntax error at line " + to_string(line) + " : Port is reserved for minecraft as a vital survival function.");
                if (port < 0 || port > 65535)
                    throw std::runtime_error("Syntax error at line " + to_string(line) + " : Port number must be between 0 and 65535.");
            }
            else
                throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected ':' after 'port'.");
        }
        else if (words[i] == HOST)
        {
            if (i + 1 < words.size() && words[i + 1] == EQUAL)
            {
                i++;
                bool waitDelimiter = false;
                while (i + 1 < words.size() && words[i + 1] != END_OF_LINE)
                {
                    i++;
                    if (words[i] == DELIMITER)
                    {
                        if (waitDelimiter)
                            throw std::runtime_error("Syntax error at line " + to_string(line) + " : Unexpected delimiter.");
                        waitDelimiter = true;
                    }
                    else
                    {
                        if (!waitDelimiter)
                            throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected a delimiter.");
                        waitDelimiter = false;
                    }
                }
                if (i + 1 >= words.size())
                    throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected a hostname after ':'.");
            }
            else
                throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected ':' after 'host'.");
        }
        else if (words[i] == ERROR_PAGES)
        {

        }
        else if (words[i] == CLIENT_MAX_BODY_SIZE)
        {
            if (i + 1 < words.size() && words[i + 1] == EQUAL)
            {
                i++;
                if (i + 1 >= words.size())
                    throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected a number after ':'.");
            }
            else
                throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected ':' after 'client_max_body_size'.");
        }
        else
            throw std::runtime_error("Syntax error at line " + to_string(line) + " : Unexpected directive.");
    }
    else if (!braces.empty() && braces.top() == ROUTE)
    {

    }
    else
        throw std::runtime_error("Syntax error at line " + to_string(line) + " : Unexpected directive.");
}

void checkBraces(const std::vector<std::string> &words)
{
    size_t line = 0;
    std::stack<std::string> braces;

    for (size_t i = 0; i < words.size(); i++)
    {
        if (words[i] == SERVER)
        {
            if (i + 1 < words.size() && words[i + 1] == OPEN_BRACE)
            {
                braces.push(SERVER);
                i++;
            }
            else
                throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected '{' after 'server'.");
        }
        else if (words[i] == ROUTE)
        {
            if (i + 1 < words.size() && words[i + 1] == OPEN_BRACE)
            {
                braces.push(ROUTE);
                i++;
            }
            else
                throw std::runtime_error("Syntax error at line " + to_string(line) + " : Expected '{' after 'route'.");
        }
        else if (words[i] == OPEN_BRACE)
        {
            throw std::runtime_error("Syntax error at line " + to_string(line) + " : Unexpected '{'.");
        }
        else if (words[i] == CLOSE_BRACE)
        {
            if (braces.empty())
                throw std::runtime_error("Syntax error at line " + to_string(line) + " : Unexpected '}'.");
            if (braces.top() == SERVER || braces.top() == ROUTE)
                braces.pop();
            else
                throw std::runtime_error("Syntax error at line " + to_string(line) + " : Unexpected '}'.");
        }
        else
            line++;
    }
}

void preParsing(std::ifstream &file)
{
    std::vector<std::string> words = split(file);
    checkBraces(words);
}

int main()
{
    std::ifstream configFile("example.conf");
    if (!configFile.is_open())
    {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier de configuration." << std::endl;
        return 1;
    }

    try
    {
        preParsing(configFile);
        std::cout << "Syntaxe correcte." << std::endl;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}