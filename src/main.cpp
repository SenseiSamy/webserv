#include "Response.hpp"
#include "Server.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <strings.h>

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    Server server(argv[1]);
	server.run();
    return 0;
}
