#include "Server.hpp"

#include "Response.hpp"

#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/epoll.h>
#include <unistd.h>

/* --------------- Server --------------- */
Server::Server(const std::string& path) : _path(path)
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

server_data* Server::get_server_to_connect(int sock_fd)
{
    for (std::size_t i = 0; i < _servers.size(); ++i)
    {
        if (sock_fd == _servers[i]._listen_fd)
            return (&_servers[i]);
    }
    return (NULL);
}

server_data& Server::get_server_from_request(Request req)
{
    std::string host = req.get_headers_key("Host");
    int port;
    std::size_t sep;
    std::vector<server_data>::iterator it;
    if (host.empty())
        return (_servers[0]);
    sep = host.find(':');
    if (sep == std::string::npos)
        return (_servers[0]);
    std::stringstream ss(host.substr(sep + 1));
    ss >> port;
    host = host.substr(0, sep);

    for (it = _servers.begin(); it != _servers.end(); ++it)
    {
        if (host == it->host && port == it->port)
            return (*it);
    }
    for (it = _servers.begin(); it != _servers.end(); ++it)
    {
        for (std::size_t i = 0; i < it->server_names.size(); ++i)
            if (host == it->server_names[i])
                return (*it);
    }
    return (_servers[0]);
}

void Server::print_log(Request& req, server_data& server) const
{
    std::stringstream ss(req.get_request());
    std::string line;
    const char* box_color = "\e[38;2;255;148;253m";

    std::cout << box_color << "╭─ Request to server " << req.get_headers_key("Host") << " (" << server.host << ":"
              << server.port << ")\e[0m" << std::endl;
    while (std::getline(ss, line))
    {
        line.erase(line.size() - 1);
        if (!line.empty())
            std::cout << box_color << "│ \e[0m" << line << std::endl;
    }
    std::cout << box_color << "╰───────────────────\e[0m" << std::endl;
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
            server_data* server = get_server_to_connect(fd);
            if (server != NULL)
            {
                client_sock = accept(server->_listen_fd, (struct sockaddr*)&client_addr, &client_addr_size);
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
                    request.append(buffer, count);
                }
                if (count == -1 && errno != EAGAIN)
                {
                    std::cerr << "run: read: " << strerror(errno) << std::endl;
                    close(fd);
                }
                else
                {
                    Request req(request);
                    server_data& server = get_server_from_request(req);
                    print_log(req, server);
                    Response response(req, server);
                    send(fd, response.to_string().c_str(), response.to_string().size(), 0);
                    close(fd);
                }
            }
        }
    }
    return 0;
}