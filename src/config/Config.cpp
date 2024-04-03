#include "Config.hpp"
#include <fstream>
#include <iterator>

Config::Config(const std::string &path) : _path(path)
{
    _file.open(_path);
    if (!_file.is_open() || _file.bad())
    {
        throw std::runtime_error("Error: could not open file " + _path);
    }
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

const std::vector<Config::ServerData> &Config::getServers() const
{
    return _servers;
}

const std::string &Config::getPath() const
{
    return _path;
}

void Config::tokenize()
{
    std::string line;
    while (std::getline(_file, line))
    {
        if (line.empty())
            continue;
        std::string::size_type pos = line.find('#');
        if (pos != std::string::npos)
            line = line.substr(0, pos);
    }
}