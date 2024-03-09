#include "main.hpp"
#include "logger.hpp"
#include "cgi.hpp"
#include <fstream>

int main(void)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
        log(FATAL, "socket error: " + std::string(strerror(errno)));

    int optval = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        log(FATAL, "setsockopt error: " + std::string(strerror(errno)));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bzero(&server_addr.sin_zero, sizeof(server_addr.sin_zero));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        log(FATAL, "bind error: " + std::string(strerror(errno)));

    listen(server_sock, 5);
    std::stringstream ss;
    ss << "server started on port " << PORT;
    log(INFO, ss.str());

    while (true)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &sin_len);
        if (client_sock < 0)
        {
            log(ERROR, "accept error: " + std::string(strerror(errno)));
            continue;
        }

        // take the request on string and parse it
        char request[2048] = {0};
        int bytes_read = recv(client_sock, request, 2048, 0);
        if (bytes_read < 0)
        {
            log(ERROR, "recv error: " + std::string(strerror(errno)));
            continue;
        }

        std::string req(request);
        log(INFO, "request: \n" + req);

		if (isCGIrequest(req))
		{
			if (handleCGI(req, client_sock) == EXIT_FAILURE)
				std::cerr << "An error happened during the cgi execution :(\n";
		}
		else
		{
			Response rep(client_sock, req.c_str());
       		rep.sendResponse();
		}
		close(client_sock);
    }

    close(server_sock);
    return 0;
}
