#pragma once

#include <unordered_map>
#include "TCPConnection.hpp"
#include "IdPool.hpp"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class Server
{
public:
    Server(boost::asio::io_service &io_service)
        : acceptor(io_service, tcp::endpoint(tcp::v4(), 4444))   
        , udpSocket(io_service, udp::endpoint(udp::v4(), 4444))
        , idPool(16)
    {
        UDPInit(io_service);
    }

private:
    void StartAccepting()
    {
        SharedPtr<TCPConnection> newConnection = TCPConnection::Create(acceptor.get_io_service(), &tcpMessageChannel);
        acceptor.async_accept(
            newConnection->GetSocket(),
            boost::bind(&Server::tcpHandleAccept, this, newConnection, boost::asio::placeholders::error)
        );
    }

    void tcpHandleAccept(SharedPtr<TCPConnection> newConnection, const boost::system::error_code &error);

    bool UDPInit(boost::asio::io_service &io_service)
    {        
        UDPReceive();
    }

    void UDPSend(UDPMessage &msg, udp::endpoint udpEndpoint)
    {
        memcpy(udpSendBuffer.c_array(), reinterpret_cast<uint8_t*>(&msg), sizeof(UDPMessage));
        udpSocket.async_send_to(
            boost::asio::buffer(udpSendBuffer),
            udpEndpoint,
            boost::bind(&Server::udpHandleSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        );
        udpBusy = true; // We'll flip this back off after it's sent
    }

    void UDPReceive()
    {
        udpSocket.async_receive(
            boost::asio::buffer(udpRecvBuffer),
            boost::bind(&Server::udpHandleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        );
    }

    void udpHandleReceive(const boost::system::error_code &error, std::size_t bytesTransferred);
    void udpHandleSend(const boost::system::error_code &error, std::size_t bytesTransferred);

    boost::array<uint8_t, sizeof(UDPMessage)> udpRecvBuffer;
    boost::array<uint8_t, sizeof(UDPMessage)> udpSendBuffer;
    std::unordered_map < uint8_t, udp::endpoint > udpConnections;
    std::unordered_map < uint8_t, SharedPtr<TCPConnection> > tcpConnections;
    udp::socket udpSocket;
    tcp::acceptor acceptor;
    bool udpBusy;

    IdPool idPool;

    Channel<UDPMessage> udpMessageChannel;
    Channel<TCPMessage> tcpMessageChannel;
};
