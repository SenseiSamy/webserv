#include "Server.hpp"

void Server::increment_index()
{
    if (_current_word >= _content_file[_current_line].size())
    {
        ++_current_line;
        _current_word = 0;
    }
    else
        ++_current_word;
}

void Server::decrement_index()
{
    if (_current_word == 0)
    {
        --_current_line;
        _current_word = _content_file[_current_line].size();
    }
    else
        --_current_word;
}

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
        increment_index();
    if (_current_line >= _content_file.size())
        return "";
    return _content_file[_current_line][_current_word++];
}

const std::string Server::previous_word()
{
    if (_current_line >= _content_file.size())
        return "";
    if (_current_word == 0)
        decrement_index();
    if (_current_line >= _content_file.size())
        return "";
    return _content_file[_current_line][_current_word--];
}
