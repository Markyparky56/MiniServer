#include "TCPConnection.hpp"
#include <iostream>

void TCPConnection::Send(TCPMessage &msg)
{
    memcpy(tcpSendBuffer.c_array(), reinterpret_cast<uint8_t*>(&msg), sizeof(TCPMessage));
    socket.async_send(
        boost::asio::buffer(tcpSendBuffer),
        boost::bind(&TCPConnection::tcpHandleSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
    tcpBusy = true;
}

void TCPConnection::tcpHandleReceive(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    if (!error)
    {
        assert(bytesTransferred == sizeof(TCPMessage)); // Check the message we just received is the right size
        TCPMessage *recvdMsg = reinterpret_cast<TCPMessage*>(tcpRecvBuffer.c_array());
        // Send it down the message channel to be handled byt he main loop
        tcpMessageChannel->Write(*recvdMsg);
    }
    else
    {
        std::cout << "Error: " << error.message() << std::endl;
#ifdef _DEBUG
        abort();
#endif
    }
}

void TCPConnection::tcpHandleSend(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    tcpBusy = false;
}
