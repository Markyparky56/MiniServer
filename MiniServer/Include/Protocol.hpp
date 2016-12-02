#pragma once
#include <cstdint>
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

// Vector3s are already 4 byte aligned due to them using floats
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
    // Data to setup an endpoint for the udp messages
    char host[16]; // Address
    char service[5]; // Port
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageIWantToConnectIPv6Data
{
    // Data to setup an endpoint for the udp messages
    char host[46]; // Address (ipv6 address can be looooong)
    char service[5]; // Port
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageYouAreConnectedData
{
    uint8_t id; // The id assigned to the newly connected client
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageIAmDisconnectingData
{
    uint8_t id; 
};
#pragma pack(pop)

#pragma pack(push, 1) // Pack as tightly as possible to reduce bandwidth
struct TCPMessageConnectTellData
{
    PlayerRecord newPlayer;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageDisconnectTellData
{
    uint8_t id;
    DisconnectType disconnectType;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessageSnapshotData
{
    uint8_t numRecords; // The number of items actually sent in the array
    PlayerRecord records[16]; // 16 should technically be the maximum number of users on the server
    // A way to cut this down would be to only send deltas, but this'll work for now
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessagePingPongData
{
    // Empty
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCPMessage
{
    TCPMessageType type;
    uint64_t unixTimestamp;
    union TCPMessageData
    {
        TCPMessageIWantToConnectIPv4Data ipv4ConnectData;
        TCPMessageIWantToConnectIPv6Data ipv6ConnectData;
        TCPMessageYouAreConnectedData youAreConnectedData;
        TCPMessageIAmDisconnectingData iAmDisconnectingData;
        TCPMessageConnectTellData connectTellData;
        TCPMessageDisconnectTellData disconnectTellData;
        TCPMessageSnapshotData snapshotData;
        TCPMessagePingPongData pingPongData;
    } data;
};
#pragma pack(pop)

#define TCPMessageSize sizeof(TCPMessage);

/*************************** Protocol Over UDP ***************************/
enum class UDPMessageType : uint8_t
{
    PlayerUpdate,
    ActuallyUpdate,
    StillThere,
    StillHere
};

#pragma pack(push, 1)
struct PlayerUpdateData
{
    PlayerRecord playerData;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ActuallyUpdate
{
    PlayerRecord playerData;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct StillThereData
{
    // Empty
};
#pragma pack(pop)

#pragma pack(push, 1)
struct StillHereData
{
    uint8_t id;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UDPMessage
{
    UDPMessageType type;
    uint64_t unixTimestamp;
    union UDPMessageData
    {
        PlayerUpdateData playerUpdateData;
        ActuallyUpdate actuallyUpdateData;
        StillThereData stillThereData;
        StillHereData stillHereData;
    } data;
};
#pragma pack(pop)

#define UDPMessageSize sizeof(UDPMessage)
