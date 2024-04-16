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
	try
	{
		Server server(argv[1]);
		server.run();
	}
	catch (const std::exception&)
	{
        return 1;
    }
    return 0;
}
