#pragma once
#include <unordered_map>
#include "TCPConnection.hpp"
#include "IdPool.hpp"
#include <thread>
#include <functional>
#include <boost/date_time/posix_time/posix_time.hpp> // For async timers


using pThread = UniquePtr<std::thread>;
using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class Server : public boost::enable_shared_from_this<Server>
{
public:
    Server(boost::asio::io_service &io_service);
    ~Server();

    // Tick is called each frame to process any messages sitting in the message channels
    // Tick can respond to messages, but otherwise messages are sent on a timer 
    bool Tick();

private:
    void ioServiceThreadFunc()
    {
        ioService->run();
    }

    void StartAccepting();
    void tcpHandleAccept(SharedPtr<TCPConnection> newConnection, const boost::system::error_code &error);

    void udpInit(boost::asio::io_service &io_service);
    void udpSend(UDPMessage &msg, udp::endpoint udpEndpoint);
    void udpReceive();
    void udpHandleReceive(const boost::system::error_code &error, std::size_t bytesTransferred);
    void udpHandleSend(const boost::system::error_code &error, std::size_t bytesTransferred);
    void udpHandleResolve(const boost::system::error_code &error, udp::resolver::iterator endpointIter, const uint8_t id);


    void SendSnapshots();

    boost::asio::deadline_timer tcpSnapshotTimer;
    bool timerActive;

    boost::array<uint8_t, sizeof(UDPMessage)> udpRecvBuffer;
    boost::array<uint8_t, sizeof(UDPMessage)> udpSendBuffer;
    std::array<udp::endpoint, 16> udpConnections;
    std::array<SharedPtr<TCPConnection>, 16> tcpConnections;
    boost::asio::io_service *ioService;
    udp::socket udpSocket;
    udp::endpoint remoteEndpoint;
    tcp::acceptor acceptor;

    IdPool idPool;

    Channel<UDPMessage, std::stack<UDPMessage> > udpMessageChannel;
    Channel<TCPMessage, std::queue<TCPMessage> > tcpMessageChannel;

    struct PlayerRecordHistory // Name is maybe a little ambiguous, tracks the last time a record was updated, and if it has been updated this frame
    {
        bool UpdatedThisFrame;
        uint64_t timestamp;
    };

    std::array<PlayerRecord, 16> playerRecords;
    std::array<PlayerRecord, 16> oldPlayerRecords;
    std::array<bool, 16> activePlayers;
    std::array<PlayerRecordHistory, 16> playerRecordHistory;

    pThread ioServiceThread;
};
using pServer = UniquePtr<Server>;
