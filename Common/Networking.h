#pragma once

#include <string>
#include "Protocol.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>

    class WSAHandler {
    public:
        WSAHandler();
        ~WSAHandler();
    };

#else
    #include <sys/socket.h>
    #define INVALID_SOCKET -1
#endif


struct Message
{
    std::string ip;
    unsigned short port;
    Packet packet;

    bool empty() {
        return ip.empty();
    }

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
#ifdef _WIN32
    SOCKET sock = INVALID_SOCKET; 
#else
    int sock = INVALID_SOCKET; 
#endif
    static constexpr std::string localhost = "127.0.0.1";
};