#include <iostream>
#include "Server.hpp"

using pThread = UniquePtr<std::thread>;

int main()
{
    boost::asio::io_service io_service; // Odd design choice to declare io_service here, but it works
    pServer server = MakeUnique<Server>(io_service);
    while (true)
    {
        server->Tick();
        //boost::asio::deadline_timer t(io_service, boost::posix_time::millisec(50)); // There has to be a better way to do this
    }
    return 0;
}
