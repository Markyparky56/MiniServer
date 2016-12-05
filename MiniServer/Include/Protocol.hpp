#pragma once
#include <cstdint>
#include <ctime>
#include "Transform.hpp"

/*************************** Protocol Over TCP ***************************/
enum class TCPMessageType : uint8_t // Might as well keep these small since we don't need to have a million message types
{
    IWantToConnectIPv4,
    IWantToConnectIPv6, // This is not going to be used, but I'm including it to show off how clever I am.
    YouAreConnected,
    IAmDisconnecting,
    ConnectTell,
    DisconnectTell, 
    Snapshot,
    Ping, // Only implement this if you have time
    Pong // Response to a ping, workout roundtrip time
};

enum class  DisconnectType : uint8_t
{
    Standard // Could be expanded to include things like connection timeout or being kicked for too high a ping
};

#pragma pack(push, 1)
struct PlayerRecord
{
    uint8_t id;
    Transform transform;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageIWantToConnectIPv4Data
{
    TCPMessageIWantToConnectIPv4Data() {}
    TCPMessageIWantToConnectIPv4Data(const uint8_t InId, const char InHost[], const char InService[])
    {
        id = InId;
        memcpy(host, InHost, 16);
        memcpy(service, InService, 5);
    }
    // Data to setup an endpoint for the udp messages
    uint8_t id;
    char host[16]; // Address
    char service[5]; // Port
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageIWantToConnectIPv6Data
{
    TCPMessageIWantToConnectIPv6Data() {}
    TCPMessageIWantToConnectIPv6Data(const uint8_t InId, const char InHost[], const char InService[])
    {
        id = InId;
        memcpy(host, InHost, 46);
        memcpy(service, InService, 5);
    }
    // Data to setup an endpoint for the udp messages
    uint8_t id;
    char host[46]; // Address (ipv6 address can be looooong)
    char service[5]; // Port
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageYouAreConnectedData
{
    TCPMessageYouAreConnectedData() {}
    TCPMessageYouAreConnectedData(uint8_t InId) : id(InId) {}
    uint8_t id; // The id assigned to the newly connected client
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageIAmDisconnectingData
{
    TCPMessageIAmDisconnectingData() {}
    TCPMessageIAmDisconnectingData(uint8_t InId) : id(InId) {}
    uint8_t id; 
};
#pragma pack(pop)

#pragma pack(push, 1) // Pack as tightly as possible to reduce bandwidth
struct TCPMessageConnectTellData
{
    TCPMessageConnectTellData() {}
    TCPMessageConnectTellData(PlayerRecord &record) : newPlayer(record) {}
    PlayerRecord newPlayer;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageDisconnectTellData
{
    TCPMessageDisconnectTellData() {}
    TCPMessageDisconnectTellData(uint8_t InId, DisconnectType InType) : id(InId), disconnectType(InType) {}
    uint8_t id;
    DisconnectType disconnectType;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageSnapshotData
{
    TCPMessageSnapshotData() {}
    TCPMessageSnapshotData(PlayerRecord InRecords[16])
    {
        memcpy(&records, &InRecords, 16);
    }
    PlayerRecord records[16]; // 16 should technically be the maximum number of users on the server
    // A way to cut this down would be to only send deltas, but this'll work for now
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessagePingPongData
{
    TCPMessagePingPongData() {}
    // Empty
};
#pragma pack(pop)

union TCPMessageData
{
    TCPMessageData() {}
    TCPMessageIWantToConnectIPv4Data ipv4ConnectData;
    TCPMessageIWantToConnectIPv6Data ipv6ConnectData;
    TCPMessageYouAreConnectedData youAreConnectedData;
    TCPMessageIAmDisconnectingData iAmDisconnectingData;
    TCPMessageConnectTellData connectTellData;
    TCPMessageDisconnectTellData disconnectTellData;
    TCPMessageSnapshotData snapshotData;
    TCPMessagePingPongData pingPongData;
};

#pragma pack(push, 1)
struct TCPMessage
{
    TCPMessageType type;
    uint64_t unixTimestamp;
    TCPMessageData data;
};
#pragma pack(pop)

#define TCPMessageSize sizeof(TCPMessage)

/*************************** Protocol Over UDP ***************************/
enum class UDPMessageType : uint8_t
{
    PlayerUpdate,
    ActuallyUpdate,
    StillThere,
    StillHere
};

enum class UDPMessageSender : uint8_t
{
    Client,
    Server
};

#pragma pack(push, 1)
struct UDPPlayerUpdateData
{
    PlayerRecord playerData;
    UDPMessageSender sender;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UDPActuallyUpdate
{
    PlayerRecord playerData;
    UDPMessageSender sender;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UDPStillThereData
{
    UDPMessageSender sender;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UDPStillHereData
{
    uint8_t id;
    UDPMessageSender sender;
};
#pragma pack(pop)

union UDPMessageData
{
    UDPMessageData() {}
    UDPPlayerUpdateData playerUpdateData;
    UDPActuallyUpdate actuallyUpdateData;
    UDPStillThereData stillThereData;
    UDPStillHereData stillHereData;
};

#pragma pack(push, 1)
struct UDPMessage
{
    UDPMessageType type;
    uint64_t unixTimestamp;
    UDPMessageData data;
};
#pragma pack(pop)

#define UDPMessageSize sizeof(UDPMessage)
