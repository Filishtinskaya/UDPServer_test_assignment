#pragma once

#include <string>

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

class Socket
{
public:
    Socket(unsigned short port);

    struct ReceivedMsg
    {
        std::string sourceIp;
        unsigned short sourcePort;
        std::string message;
    };
    ReceivedMsg receive() const;

    void send(std::string ip, unsigned short port, std::string message) const;

    ~Socket();
private:
    SOCKET sock = INVALID_SOCKET; 
    static constexpr std::string localhost = "127.0.0.1";
    static constexpr size_t MTU = 1200;
};