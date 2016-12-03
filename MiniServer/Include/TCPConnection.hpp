#pragma once
#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include <boost\enable_shared_from_this.hpp>
#include <boost\shared_ptr.hpp>
#include <boost\array.hpp>
#include "Channel.hpp"
#include "Protocol.hpp"
#include "UniquePtr.hpp"
#include "SharedRef.hpp"

using boost::asio::ip::tcp;

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
{
public:
    static SharedPtr<TCPConnection> Create(boost::asio::io_service &io_service, Channel<TCPMessage> *InTcpMessageChannel)
    {
        return MakeShareable(new TCPConnection(io_service, InTcpMessageChannel));
    }

    tcp::socket &GetSocket() { return socket; }

    void Send(TCPMessage &msg);
    void Close()
    {
        socket.close();
    }
    
private:
    TCPConnection(boost::asio::io_service &io_service, Channel<TCPMessage> *InTcpMessageChannel)
        : socket(io_service)
        , tcpMessageChannel(InTcpMessageChannel)
    {
    }

    void tcpHandleReceive(const boost::system::error_code &error, std::size_t bytesTransferred);
    void tcpHandleSend(const boost::system::error_code &error, std::size_t bytesTransferred);

    boost::array<uint8_t, sizeof(TCPMessage)> tcpSendBuffer;
    boost::array<uint8_t, sizeof(TCPMessage)> tcpRecvBuffer;

    Channel<TCPMessage> *tcpMessageChannel;

    tcp::socket socket;
    bool tcpBusy;
};
