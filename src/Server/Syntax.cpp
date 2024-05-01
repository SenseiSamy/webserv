#include "Server.hpp"

void Server::syntax_brackets()
{
    size_t open_brackets = 0;
    size_t close_brackets = 0;
    std::string word = next_word();

    while (word != "")
    {
        if (word == "server" || word == "route")
        {
            word = next_word();
            if (word != "{")
                throw std::runtime_error("Syntax error: Expected '{' after " + word + "at line " + to_string(_current_line));
            ++open_brackets;
        }
        else if (word == "}")
            ++close_brackets;
    }

    if (open_brackets != close_brackets)
        throw std::runtime_error("Syntax error: Missing '}' at line " + to_string(_current_line));
}
