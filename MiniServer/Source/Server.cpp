#include "Server.hpp"
#include <iostream>

Server::Server(boost::asio::io_service & io_service)
    : ioService(&io_service)
    , acceptor(io_service, tcp::endpoint(tcp::v4(), 4443))
    , udpSocket(io_service)
    , idPool(16)
    , tcpSnapshotTimer(io_service, boost::posix_time::millisec(200)) // Arbitrary, but every 1/5s feels reasonable
{
    PlayerRecord fillerRecord;
    fillerRecord.id = -1;
    fillerRecord.transform.SetPosition(Vector3(0.f, 0.f, 0.f));
    playerRecords.fill(fillerRecord);
    oldPlayerRecords.fill(fillerRecord);
    activePlayers.fill(false);
    playerRecordHistory.fill({ false, static_cast<uint64_t>(std::time(nullptr)) });

    udpInit(io_service);
    tcpConnections.fill(SharedPtr<TCPConnection>(nullptr));
    StartAccepting();
    ioServiceThread = MakeUnique<std::thread>(std::mem_fun(&Server::ioServiceThreadFunc), this);
}

Server::~Server()
{
    ioService->stop();
    ioService->reset(); // Probably not needed, but means if some how the server is destroyed but the io_service is reused it'll run again
    ioServiceThread->join();
}

void Server::StartAccepting()
{
    SharedPtr<TCPConnection> newConnection = TCPConnection::Create(acceptor.get_io_service(), &tcpMessageChannel);
    acceptor.async_accept(
        newConnection->GetSocket(),
        boost::bind(&Server::tcpHandleAccept, this, newConnection, boost::asio::placeholders::error)
    );
}

void Server::SendSnapshots()
{
    for (auto &connection : tcpConnections)
    {
        if (connection.IsValid())
        {
            // Send them a snapshot
            TCPMessageData snapshot;
            snapshot.snapshotData =
            {
                playerRecords.data()
            };
            TCPMessage snapshotMsg =
            {
                TCPMessageType::Snapshot,
                static_cast<uint64_t>(std::time(nullptr)),
                snapshot
            };
            connection->Send(snapshotMsg);
            std::cout << "Snapshot sent" << std::endl;
        }
    }
}

void Server::udpInit(boost::asio::io_service &io_service)
{
    udpSocket = udp::socket(io_service, udp::endpoint(udp::v4(), 4443));
    udpReceive();
}

void Server::udpSend(UDPMessage &msg, udp::endpoint udpEndpoint)
{
    memcpy(udpSendBuffer.c_array(), reinterpret_cast<uint8_t*>(&msg), sizeof(UDPMessage));
    udpSocket.async_send_to(
        boost::asio::buffer(udpSendBuffer),
        udpEndpoint,
        boost::bind(&Server::udpHandleSend, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
    std::cout << "UDP Message supposedly sent" << std::endl;
}

void Server::udpReceive()
{
    udpSocket.async_receive(
        boost::asio::buffer(udpRecvBuffer),
        boost::bind(&Server::udpHandleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
    );
}

void Server::udpHandleReceive(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    if (!error)
    {
        assert(bytesTransferred == UDPMessageSize); // Check the message we just received is the right size
        UDPMessage *recvdMsg = reinterpret_cast<UDPMessage*>(udpRecvBuffer.c_array());
        udpMessageChannel.Write(*recvdMsg);
        std::cout << "UDP Message received" << std::endl;
    }
    else
    {
        std::cout << "Error: " << error.message() << std::endl;
#ifdef _DEBUG
        abort();
#endif
    }

    udpReceive(); // Back to the grind...
}

void Server::udpHandleSend(const boost::system::error_code & error, std::size_t bytesTransferred)
{
    if (!error)
    {
        std::cout << "UDP Message sent! " << std::endl;
    }
    else
    {
        std::cout << "Error: " << error.message() << std::endl;
    }
}

void Server::udpHandleResolve(const boost::system::error_code & error, udp::resolver::iterator endpointIter, const uint8_t id)
{
    if (!error)
    {
        udpConnections[id] = *endpointIter; // Store the endpoint
    }
    else
    {
        // Resolve failed for some reason
        std::cout << "Error: " << error.message() << std::endl;
#ifdef _DEBUG
        abort();
#endif
    }
}

void Server::tcpHandleAccept(SharedPtr<TCPConnection> newConnection, const boost::system::error_code & error)
{
    if (!error)
    {
        // Give the new connection an id and store it
        uint8_t id = static_cast<uint8_t>(idPool.GetNextID());
        tcpConnections[id].Reset(); // Make sure we clear anything which may be lingering
        tcpConnections[id] = newConnection;
        newConnection->StartReceive();
        // Tell the new client who they are
        TCPMessageData data;
        data.youAreConnectedData =
        {
            id
        };
        TCPMessage response =
        {
            TCPMessageType::YouAreConnected,
            static_cast<uint64_t>(std::time(nullptr)),
            data
        };
        newConnection->Send(response);
#ifdef _DEBUG
        std::cout << "ID: " << static_cast<char>(id + 48) << " assigned to new connection" << std::endl;
#endif

        // Send them a snapshot
        TCPMessageData snapshot;
        snapshot.snapshotData =
        {
            playerRecords.data()
        };
        TCPMessage snapshotMsg =
        {
            TCPMessageType::Snapshot,
            static_cast<uint64_t>(std::time(nullptr)),
            snapshot
        };
        newConnection->Send(snapshotMsg);

        // Communicate the new connection to all the other clients
        TCPMessageData newConData;
        newConData.connectTellData =
        {
            playerRecords[id]
        };
        TCPMessage newConMsg =
        {
            TCPMessageType::ConnectTell,
            static_cast<uint64_t>(std::time(nullptr)),
            newConData
        };

        for (int i = 0; i < 16; i++)
        {
            if (activePlayers[i] && i != id) // No point telling the new connection about itself
            {
                tcpConnections[i]->Send(newConMsg);
            }
        }

        if (!timerActive)
        {
            tcpSnapshotTimer.async_wait(boost::bind(&Server::SendSnapshots, this));
            timerActive = true;
        }
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


bool Server::Tick()
{
    for (auto &record : playerRecordHistory)
    {
        record.UpdatedThisFrame = false;
    }
    
    while (!tcpMessageChannel.Empty())
    {
        TCPMessage msg = tcpMessageChannel.Read();
        switch (msg.type)
        {
        case TCPMessageType::IWantToConnectIPv4:
        {
            TCPMessageIWantToConnectIPv4Data data = msg.data.ipv4ConnectData;
            udp::resolver::query query(udp::v4(), data.host, data.service);
            udp::resolver resolver(*ioService);
            resolver.async_resolve(
                query,
                boost::bind(&Server::udpHandleResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator, data.id)
            );
            break;
        }
        case TCPMessageType::IWantToConnectIPv6:
        {
            TCPMessageIWantToConnectIPv6Data data = msg.data.ipv6ConnectData;
            udp::resolver::query query(udp::v6(), data.host, data.service);
            udp::resolver resolver(*ioService);
            resolver.async_resolve(
                query,
                boost::bind(&Server::udpHandleResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator, data.id)
            );
            break;
        }
        case TCPMessageType::IAmDisconnecting:
        {
            TCPMessageIAmDisconnectingData data = msg.data.iAmDisconnectingData;
            tcpConnections[data.id]->Close();
            tcpConnections[data.id].Reset();
            activePlayers[data.id] = false;
            idPool.ReturnID(data.id);

            // Tell all the other clients
            TCPMessageData disconnectData;
            disconnectData.disconnectTellData =
            {
                data.id,
                DisconnectType::Standard
            };
            TCPMessage disconMsg =
            {
                TCPMessageType::DisconnectTell,
                static_cast<uint64_t>(std::time(nullptr)),
                disconnectData
            };

            for (int i = 0; i < 16; i++)
            {
                if (activePlayers[i])
                {
                    tcpConnections[i]->Send(disconMsg);
                }
            }

            break;
        }
        case TCPMessageType::Pong:
        {
            // Work out roundtrip time here
            break;
        }
        default:
            std::cout << "Unrecognised TCP Message Type!" << std::endl;
            break;
        }
    }

    while (!udpMessageChannel.Empty())
    {
        UDPMessage msg = udpMessageChannel.Read();
        switch (msg.type)
        {
        case UDPMessageType::PlayerUpdate:
        {
            PlayerRecord &newRecord = msg.data.actuallyUpdateData.playerData;
            if (playerRecordHistory[newRecord.id].UpdatedThisFrame)
            {
                if (playerRecordHistory[newRecord.id].timestamp > msg.unixTimestamp) // Stored record is newer than the one in the message
                {
                    break;
                }
                // Else the one we're looking at is newer so let's store it
            }
            oldPlayerRecords[newRecord.id] = playerRecords[newRecord.id]; // Save the old record
            playerRecords[newRecord.id] = newRecord; // Store the new record
            playerRecordHistory[newRecord.id].UpdatedThisFrame = true;
            playerRecordHistory[newRecord.id].timestamp = msg.unixTimestamp;
            
            Vector3 playerPos = newRecord.transform.GetPosition();
            std::cout << "PlayerID: " << static_cast<char>(newRecord.id+48) << " Pos(" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")" << std::endl;


            // Inform all the other connected clients of this new data
            UDPMessage newMsg = msg;
            msg.unixTimestamp = static_cast<uint64_t>(std::time(nullptr)); // Update the timestamp
            msg.data.playerUpdateData.sender = UDPMessageSender::Server;


            for (int id = 0; id < 16; id++)
            {
                if (activePlayers[id] && id != newRecord.id)
                {
                    udpSend(newMsg, udpConnections[id]);
                }
            }

            break;
        }
        case UDPMessageType::StillHere:
        {
            break;
        }
        default:
            std::cout << "Unrecognised UDP Message Type!" << std::endl;
            break;
        }
    }

    return true;
}
