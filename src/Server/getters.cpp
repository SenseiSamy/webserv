#include "Server.hpp"

const std::vector<server_data> Server::get_servers() const
{
    return _servers;
}

const std::vector<std::vector<std::string> > Server::get_file_config_content() const
{
    return _file_config_content;
}

const std::string& Server::get_path() const
{
    return _path;
}
