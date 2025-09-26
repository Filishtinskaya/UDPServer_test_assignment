#pragma once

#include <string>
#include "Protocol.h"

#ifdef _WIN32
    #include <winsock2.h>

    class WSAHandler {
    public:
        WSAHandler();
        ~WSAHandler();
    };

#else
    #include <sys/socket.h>

#endif
#include <ws2tcpip.h>

struct Message
{
    std::string ip;
    unsigned short port;
    Packet packet;

    void networkEndianConversion();
};

class Socket
{
public:
    Socket(unsigned short port);

    Message receive() const;

    void send(Message&& message) const;

    ~Socket();
private:
    SOCKET sock = INVALID_SOCKET; 
    static constexpr std::string localhost = "127.0.0.1";
};