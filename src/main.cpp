#include "../inc/main.hpp"

static int init_socket(int port) {
    int server_fd;
    struct sockaddr_in server_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "socket failed" << std::endl;
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
            0) {
        std::cerr << "bind failed" << std::endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "listen failed" << std::endl;
        return -1;
    }

    return server_fd;
}

int main(void) {
    int server_fd, new_socket;
    struct sockaddr_in client_addr;

    if ((server_fd = init_socket(PORT)) < 0) {
        return -1;
    }

    int addrlen = sizeof(client_addr);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr,
                             (socklen_t *)&addrlen)) < 0) {
        std::cerr << "accept failed" << std::endl;
        return -1;
    }

    char buffer[1024] = {0};

    int valread;

    if ((valread = read(new_socket, buffer, 1024)) < 0) {
        std::cerr << "read failed" << std::endl;
        return -1;
    }

    std::cout << buffer << std::endl;
    send(new_socket, "Hello from server", 18, 0);

    close(new_socket);
    close(server_fd);

    return 0;
}
