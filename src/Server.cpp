#include "Server.hpp"
#include "HTTPRequest.hpp"
#include "Response.hpp"

/* --------------- Server --------------- */
Server::Server(const std::string &path) : _path(path)
{
    if (read_files() == -1)
        throw std::runtime_error("Error: cannot open the file " + path);
    if (_file_config_content.empty())
        throw std::runtime_error("Error: config file cannot be empty");
    if (syntax_brackets() == -1)
        throw std::runtime_error("Error: syntax error in file " + path);
    if (parsing_config() == -1)
        throw std::runtime_error("Error: parsing error in file " + path);
}

Server::~Server()
{
    _file_config_content.clear();
}

/* -------------------------------------- */

std::vector<std::string> Server::split_line(const std::string &str)
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

int Server::read_files()
{
    std::ifstream file(_path.c_str());
    if (!file.is_open() || !file.good())
        return (-1);

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

        _file_config_content.push_back(parsed_line);
    }

    file.close();
    return (0);
}

int Server::syntax_brackets()
{
    size_t open_brackets = 0;
    size_t close_brackets = 0;
    std::string last_word;

    for (size_t line = 0; line < _file_config_content.size(); ++line)
    {
        for (size_t i = 0; i < _file_config_content[line].size(); ++i)
        {
            // last word
            if (i > 0)
                last_word = _file_config_content[line][i - 1];
            else if (line > 0)
                last_word = _file_config_content[line - 1].back();
            std::string word = _file_config_content[line][i];
            if (word == "{")
            {
                ++open_brackets;
                if (last_word != "server" && last_word != "route")
                    return (std::cerr << "Syntax error: Unexpected '{' at line " << line + 1 << "." << std::endl, -1);
            }
            else if (word == "}")
                ++close_brackets;
        }
    }

    if (open_brackets != close_brackets)
        return (std::cerr << "Syntax error: Unbalanced brackets." << std::endl, -1);

    return 0;
}

int Server::parsing_routes(const std::vector<std::string> &token_args, routes_data &new_routes,
                           const std::string &current_word, size_t line)
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

int Server::parsing_server(const std::vector<std::string> &token_args, server_data &new_server,
                           const std::string &current_word, size_t line)
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

        new_server.host = token_args[0];
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

int Server::parsing_config()
{
    bool in_server = false;
    bool in_routes = false;

    server_data new_server;
    routes_data new_routes;
    for (size_t line = 0; line < _file_config_content.size(); ++line)
    {
        for (size_t i = 0; i < _file_config_content[line].size(); ++i)
        {
            std::string current_word = _file_config_content[line][i];
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
                    new_routes = routes_data();
                }
                else if (in_server)
                {
                    servers.push_back(new_server);
                    in_server = false;
                    new_server = server_data();
                }
                continue;
            }
            else if (current_word == "{")
                continue;

            ++i;
            if (_file_config_content[line][i] != "=")
                return (std::cerr << "Syntax error: Missing '=' at line: " << line + 1 << "." << std::endl, -1);
            ++i;
            std::vector<std::string> token_args;
            while (i < _file_config_content[line].size())
            {
                if (_file_config_content[line][i] == ";")
                    break;
                if (_file_config_content[line][i][0] != '"' ||
                    _file_config_content[line][i][_file_config_content[line][i].size() - 1] != '"')
                    return (std::cerr << "Syntax error: Missing '\"' at line: " << line + 1 << "." << std::endl, -1);
                // Remove quotes
                token_args.push_back(_file_config_content[line][i].substr(1, _file_config_content[line][i].size() - 2));
                ++i;
            }
            if (token_args.empty())
                return (std::cerr << "Syntax error: Missing arguments at line: " << line + 1 << "." << std::endl, -1);
            if (i < _file_config_content[line].size() - 1 && _file_config_content[line][i + 1] != ";")
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

/* --------------- Socket --------------- */

int Server::open_sockets()
{
    for (std::vector<server_data>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        it->_listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (it->_listen_fd < 0)
            return (std::cerr << "open_sockets: socket: " << strerror(errno) << std::endl, -1);

        int optval = 1;
        if (setsockopt(it->_listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
            return (std::cerr << "open_sockets: setsockopt: " << strerror(errno) << std::endl, -1);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(it->host.c_str());
        addr.sin_port = htons(it->port);
        memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));

        if (bind(it->_listen_fd, (struct sockaddr *)(&addr), sizeof(addr)) == -1)
            return (std::cerr << "open_sockets: bind: " << strerror(errno) << std::endl, -1);
        if (listen(it->_listen_fd, 5) == -1)
            return (std::cerr << "open_sockets: listen: " << strerror(errno) << std::endl, -1);

        std::cout << "Server started on \033[1;34m" << it->host << ":" << it->port << "\033[0m" << std::endl;
    }

    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1)
        return (std::cerr << "open_sockets: epoll_create: " << strerror(errno) << std::endl, -1);

    epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Edge-triggered read events
    for (std::size_t i = 0; i < servers.size(); ++i)
    {
        event.data.fd = servers[i]._listen_fd;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, servers[i]._listen_fd, &event) == -1)
            return (std::cerr << "open_sockets: epoll_ctl: " << strerror(errno) << std::endl, -1);
    }

    return 0;
}

server_data *Server::get_server_to_connect(int sock_fd)
{
    for (std::size_t i = 0; i < servers.size(); ++i)
    {
        if (sock_fd == servers[i]._listen_fd)
            return (&servers[i]);
    }
    return (NULL);
}

server_data &Server::get_server_from_request(HTTPRequest req)
{
    std::string host = req.getHeader("Host");
    int port;
    std::size_t sep;
    std::vector<server_data>::iterator it;
    if (host.empty())
        return (servers[0]);
    sep = host.find(':');
    if (sep == std::string::npos)
        return (servers[0]);
    port = std::atoi(host.substr(sep + 1, std::string::npos).c_str());
    host = host.substr(0, sep);

    for (it = servers.begin(); it != servers.end(); ++it)
    {
        if (host == it->host && port == it->port)
            return (*it);
    }
    for (it = servers.begin(); it != servers.end(); ++it)
    {
        for (std::size_t i = 0; i < it->server_names.size(); ++i)
            if (host == it->server_names[i])
                return (*it);
    }
    return (servers[0]);
}

int Server::run()
{
    epoll_event ev, events[MAX_EVENTS];
    int client_sock;
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    open_sockets();

    while (true)
    {
        int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0)
            return (-1);

        for (int n = 0; n < num_events; ++n)
        {
            const int fd = events[n].data.fd;
            server_data *server = get_server_to_connect(fd);
            if (server != NULL)
            {
                client_sock = accept(server->_listen_fd, (struct sockaddr *)&client_addr, &client_addr_size);
                if (client_sock < 0)
                {
                    std::cerr << "run: accept: " << strerror(errno) << std::endl;
                    continue;
                }
                fcntl(client_sock, F_SETFL, O_NONBLOCK);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_sock;
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_sock, &ev) == -1)
                {
                    std::cerr << "run: epoll_ctl: " << strerror(errno) << std::endl;
                    return (-1);
                }
            }
            else if ((events[n].events & EPOLLERR) || (events[n].events & EPOLLHUP) || (!(events[n].events & EPOLLIN)))
                close(fd);
            else
            {
                std::string request("");
                char buffer[1024];
                ssize_t count;
                while ((count = read(fd, buffer, sizeof(buffer) - 1)) > 0)
                {
                    buffer[count] = '\0';
                    request += buffer;
                }
                if (count == -1 && errno != EAGAIN)
                {
                    std::cerr << "run: read: " << strerror(errno) << std::endl;
                    close(fd);
                }
                else
                {
                    std::stringstream ss(request);
                    std::string line;
                    std::vector<std::string> lines;

                    while (std::getline(ss, line))
                    {
                        lines.push_back(line);
                    }

                    size_t max_digits = 0;
                    if (!lines.empty())
                    {
                        size_t max_line = lines.size() - 1;
                        while (max_line > 0)
                        {
                            max_line /= 10;
                            max_digits++;
                        }
                    }

                    std::cout << HEADER_COLOR
                              << "===================================================================" << RESET_COLOR
                              << std::endl;
                    std::cout << REQUEST_COLOR << "Request from " << inet_ntoa(client_addr.sin_addr) << ":"
                              << ntohs(client_addr.sin_port) << RESET_COLOR << std::endl;
                    for (size_t i = 0; i < lines.size(); i++)
                    {
                        std::cout << LINE_NUMBER_COLOR << std::setw(max_digits) << std::right << i << " >>> "
                                  << RESET_COLOR << lines[i] << std::endl;
                    }
                    std::cout << HEADER_COLOR
                              << "===================================================================" << RESET_COLOR
                              << std::endl;

                    HTTPRequest req(request);
                    Response response(req, get_server_from_request(req));
                    send(fd, response.toString().c_str(), response.toString().size(), 0);
                    close(fd);
                }
            }
        }
    }
    return 0;
}