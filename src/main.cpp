#include "main.hpp"
#include "logger.hpp"

void handle_client(int client_sock)
{
    const char *http_response = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 12\r\n"
                                "\r\n"
                                "Hello World!";

    write(client_sock, http_response, strlen(http_response));
}

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

        handle_client(client_sock);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
