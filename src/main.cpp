#include "Response.hpp"
#include "Server.hpp"

#include <arpa/inet.h>
#include <cstddef>
#include <fcntl.h>
#include <strings.h>


// tmp for testing
//  Config::Config(void) {
//  	Config::ServerConfig server;

// 	server.port = 4934;
// 	server.host = "127.0.0.1";
// 	this->servers.push_back(server);

// 	server.port = 36734;
// 	server.host = "127.53.86.3";
// 	this->servers.push_back(server);
// }

// const std::vector<Config::ServerConfig>& Config::getServers() const {
// 	return (servers);
// }

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
