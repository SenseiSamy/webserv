#include "Config.hpp"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <pthread.h>
#include <string>

Config::Config(const std::string &path) : _path(path)
{
    _file.open(_path);
    if (!_file.is_open() || _file.bad())
    {
        throw std::runtime_error("Error: could not open file " + _path);
    }

    tokenize();
    syntax_brackets();

    _file.close();
}

Config::Config(const Config &other)
{
    *this = other;
}

Config::~Config()
{
    if (_file.is_open())
        _file.close();
}

Config &Config::operator=(const Config &other)
{
    if (this != &other)
    {
        _path = other._path;
        _file.close();
        _file.open(other._path);
        if (!_file.is_open() || _file.bad())
            throw std::runtime_error("Error: could not open file " + other._path);
        _servers = other._servers;
    }
    return *this;
}

const std::vector<Config::Server> &Config::getServers() const
{
    return _servers;
}

const std::string &Config::getPath() const
{
    return _path;
}

int Config::is_spearator(const char c) const
{
    return (c == '"' || c == '\'' || c == '#' || c == '{' || c == '}' || c == ';' || c == '=' || c == ',' || c == '[' || c == ']');
}

void Config::tokenize()
{
    std::string line;
    size_t line_number = 0;
    std::string token;
    while (std::getline(_file, line))
    {
        line_number++;
        if (line.empty())
            continue;

        // remove comments
        std::string::size_type pos = line.find('#');
        if (pos != std::string::npos)
            line.erase(pos);

        // tokenize line
        std::string::size_type start = 0, i = 0, end = line.size();
        while (i < line.size())
        {
            while (i < end && std::isspace(line[i]))
                i++;
            start = i;
            while (i < end && !is_spearator(line[i]) && !std::isspace(line[i]))
                i++;
            token = line.substr(start, i - start);
            if (token.empty())
                continue;
            if (token == "{")
            {
            }
        }
    }
}

/*
server -> "= { };",
    port -> "= ;",
    host -> "= "";" ou "= '';",

    routes -> "= { };"


*/

int Config::syntax_brackets() const
{
    if (_tokens.empty())
        return 0;

}

int Config::syntax_tokens() const
{

}

void Config::syntax_config() const
{
    if (_tokens.empty())
        throw std::runtime_error("Error: empty configuration file");
}
