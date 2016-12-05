#include "TCPConnection.hpp"

void TCPConnection::StartReceive()
{
    socket.async_receive(
        boost::asio::buffer(tcpRecvBuffer),
        boost::bind(&TCPConnection::tcpHandleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );    
}

void TCPConnection::Send(TCPMessage &msg)
{
    memcpy(tcpSendBuffer.c_array(), reinterpret_cast<uint8_t*>(&msg), sizeof(TCPMessage));
    socket.async_send(
        boost::asio::buffer(tcpSendBuffer),
        boost::bind(&TCPConnection::tcpHandleSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
}

void TCPConnection::tcpHandleReceive(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    if (!error)
    {
        assert(bytesTransferred == TCPMessageSize); // Check the message we just received is the right size
        TCPMessage *recvdMsg = reinterpret_cast<TCPMessage*>(tcpRecvBuffer.c_array());
        // Send it down the message channel to be handled byt he main loop
        tcpMessageChannel->Write(*recvdMsg);
        std::cout << "TCP Message received" << std::endl;
    }
    else
    {
        std::cout << "Error: " << error.message() << std::endl;
#ifdef _DEBUG
        //abort();
#endif
    }
    if (socket.is_open())
    {
        StartReceive();
    }
}

void TCPConnection::tcpHandleSend(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    if (!error)
    {
        std::cout << "TCP Message sent! " << std::endl;
    }
    else
    {
        std::cout << "Error: " << error.message() << std::endl;
    }
}
