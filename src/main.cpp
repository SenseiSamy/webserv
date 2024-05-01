#include "Server.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        Server server(argv[1]);
        server.display();
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
