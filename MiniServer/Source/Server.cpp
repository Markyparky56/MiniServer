#include "..\Include\Server.hpp"
#include <iostream>

void Server::tcpHandleAccept(SharedPtr<TCPConnection> newConnection, const boost::system::error_code & error)
{
    if (!error)
    {
        // Give the new connection an id and store it
        uint8_t id = static_cast<uint8_t>(idPool.GetNextID());
        tcpConnections[id].Reset(); // Make sure we clear anything which may be lingering
        tcpConnections[id] = newConnection;
        // Tell the new client who they are
        TCPMessage response = { TCPMessageType::YouAreConnected, std::time(nullptr), { id } };
        newConnection->Send(response);
    }
    else
    {
        std::cout << "Error: " << error.message() << std::endl;
#ifdef _DEBUG
        abort();
#endif
    }

    StartAccepting(); // Wait for a new connection
}

void Server::udpHandleReceive(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    if (!error)
    {
        assert(bytesTransferred == sizeof(UDPMessage)); // Check the message we just received is the right size
        UDPMessage *recvdMsg = reinterpret_cast<UDPMessage*>(udpRecvBuffer.c_array());
        udpMessageChannel.Write(*recvdMsg);
    }
    else
    {
        std::cout << "Error: " << error.message() << std::endl;
#ifdef _DEBUG
        abort();
#endif
    }

    UDPReceive(); // Back to the grind...
}

void Server::udpHandleSend(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    udpBusy = false;
}
