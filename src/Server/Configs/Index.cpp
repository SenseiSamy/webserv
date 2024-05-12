#include "Server.hpp"

void Server::reset_index()
{
    _current_word = 0;
    _current_line = 0;
}

const std::string Server::next_word()
{
    if (_current_line >= _content_file.size())
        return "";
    if (_current_word >= _content_file[_current_line].size())
    {
        if (_current_line + 1 >= _content_file.size())
            return "";
        while (_content_file[++_current_line].empty())
            ;
        _current_word = 0;
    }
    return _content_file[_current_line][_current_word++];
}

const std::string Server::previous_word()
{
    if (_current_line == 0 && _current_word == 0)
        return "";
    if (_current_word == 0)
    {
        while (_content_file[--_current_line].empty())
            ;
        _current_word = _content_file[_current_line].size();
    }
    return _content_file[_current_line][--_current_word];
}
