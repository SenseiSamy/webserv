#include "main.hpp"

const int port = 8080;

std::string read_file(std::string filepath) 
{
    std::ifstream file(filepath.c_str());
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void handle_client(int client_sock)
{
    // const char *http_response = "HTTP/1.1 200 OK\r\n"
    //                             "Content-Type: text/plain\r\n"
    //                             "Content-Length: 22\r\n"
    //                             "\r\n"
    //                             "Hello World! what's up";

	// std::string not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";

    // write(client_sock, not_found_response.c_str(), strlen(not_found_response.c_str()));
	// write(client_sock, http_response, strlen(http_response));

	// send(client_sock, http_response, strlen(http_response), 0);
	char buffer[1024] = {0};
    read(client_sock, buffer, 1024);
    std::cout << "Request received:\n" << buffer << std::endl;
	std::string html = read_file("www/index.html");
	// std::string css = read_file("www/styles.css");

	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Type: text/html\r\n\r\n";
	response << html << "\n";

	write(client_sock, response.str().c_str(), response.str().length());
}

int main(void)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        std::cerr << "socket error: " << strerror(errno) << std::endl;
        exit(1);
    }

    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    bzero(&server_addr.sin_zero, sizeof(server_addr.sin_zero));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "bind error: " << strerror(errno) << std::endl;
        exit(2);
    }

    listen(server_sock, 5);
    std::cout << "Server is running on port " << port << std::endl;

    while (true)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &sin_len);
        if (client_sock < 0)
        {
            std::cerr << "accept error: " << strerror(errno) << std::endl;
            continue;
        }

        handle_client(client_sock);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
