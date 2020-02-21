#include <iostream>
#include "server.h"

int main(int argc, char* argv[])
{
    try
    {
        
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port>\n";
            return 1;
        }
        
        ba::io_service io_service;
        ba::ip::tcp::endpoint endpoint(ba::ip::tcp::v4(), std::atoi(argv[1]));
       // ba::ip::tcp::endpoint endpoint(ba::ip::tcp::v4(), 9000);
        Server data_server(io_service, endpoint);
        io_service.run();

    }
    catch (std::exception & e)

    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
