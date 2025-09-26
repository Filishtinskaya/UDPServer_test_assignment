#pragma once
#include <string_view>
#include <cstdint>
#include <array>
#include <vector>
#include <variant>
#include <string>
#include <bitset>
#include <assert.h>

#include <Utils.h>

enum class PacketType : uint8_t {Connect, Request, Data, Fin};

constexpr uint8_t CUR_PROTOCOL_VERSION = 1;
constexpr size_t VALUES_IN_DATA_PACKET = 125;

struct Header
{
    uint8_t protocolVersion;
    PacketType type;
};

struct SimplePacket
{
    Header header;
};

struct RequestPacket
{
    Header header;
    double value;
};

struct DataPacket
{
    Header header;
    std::array<double, VALUES_IN_DATA_PACKET> data;
};

union Packet {
    SimplePacket basic;
    RequestPacket request;
    DataPacket data;
    char msg[MTU];
};


