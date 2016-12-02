#include "TCPConnection.hpp"

void TCPConnection::Send(TCPMessage &msg)
{
    memcpy(tcpSendBuffer.c_array(), reinterpret_cast<uint8_t*>(&msg), sizeof(TCPMessage));
    socket.async_send(
        boost::asio::buffer(tcpSendBuffer),
        boost::bind(&TCPConnection::tcpHandleSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
    tcpBusy = true;
}
