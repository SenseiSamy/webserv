#include "main.hpp"
#include "logger.hpp"

int main(void)
{
    logger log;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
        log.log(FATAL, "socket error: " + std::string(strerror(errno)));

    int optval = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        log.log(FATAL, "setsockopt error: " + std::string(strerror(errno)));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bzero(&server_addr.sin_zero, sizeof(server_addr.sin_zero));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        log.log(FATAL, "bind error: " + std::string(strerror(errno)));

    listen(server_sock, 5);
    std::stringstream ss;
    ss << "server started on port " << PORT;
    log.log(INFO, ss.str());

    while (true)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &sin_len);
        if (client_sock < 0)
        {
            log.log(ERROR, "accept error: " + std::string(strerror(errno)));
            continue;
        }

        // take the request on string and parse it
        char request[2048];
        int bytes_read = recv(client_sock, request, 2048, 0);
        if (bytes_read < 0)
        {
            log.log(ERROR, "recv error: " + std::string(strerror(errno)));
            continue;
        }

        std::string req(request);
        log.log(INFO, "request: \n" + req);

        // if the file is index.html, then send the file
        if (req.find("GET /index.html") != std::string::npos || req.find("GET /") != std::string::npos)
        {
            std::ifstream file;
            file.open("www/index.html");
            if (!file.is_open())
            {
                log.log(ERROR, "file open error: " + std::string(strerror(errno)));
                continue;
            }

            std::string response;
            std::string line;
            while (std::getline(file, line))
                response += line + "\n";

            file.close();
            response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + response;
            log.log(INFO, "response: \n" + response);
            send(client_sock, response.c_str(), response.size(), 0);
        }
        else
        {
            std::string response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n";
            send(client_sock, response.c_str(), response.size(), 0);
        }

        close(client_sock);
    }

    close(server_sock);
    return 0;
}
