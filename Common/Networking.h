#pragma once

#include <string>
#include <stdexcept>
#include <tchar.h>
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif


#ifdef _WIN32

class WSAHandler {
public:
    WSAHandler() {
        WSAData wsaData;
        int wsaerr;
        WORD wVersionRequested = MAKEWORD(2, 2);
        wsaerr = WSAStartup(wVersionRequested, &wsaData);
        if (wsaerr != 0) {
            throw std::runtime_error("WSAStartup initialization error " + std::to_string(wsaerr));
        }
    }
    ~WSAHandler() {
        WSACleanup();
    }
};

#endif

class Socket
{
public:
    Socket(unsigned short port) {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sock == INVALID_SOCKET) {
            throw std::runtime_error("Error creating socket " + std::to_string(WSAGetLastError()));
        }

        sockaddr_in address = formatAddress(localhost.data(), port);

        if (bind(sock, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR) {
            throw std::runtime_error("Bind socket failed " + std::to_string(WSAGetLastError()));
        }
    }

    void receive() {
        char buffer[200] = "";
        sockaddr_in clientAddress;
        int addrLength = sizeof(clientAddress);

        int bytes_received = recvfrom(sock, buffer, 200, 0, (SOCKADDR*)&clientAddress, &addrLength);

        if (bytes_received == SOCKET_ERROR) {
            std::cout << "Error receiving data " + std::to_string(WSAGetLastError()) << std::endl;
        } else {
            std::cout << "Received datagram: " << buffer << std::endl;
        }
    }

    void send(std::string ip, unsigned short port) {
        sockaddr_in clientAddress = formatAddress(ip.data(), port);
        char buffer[200] = "test test test";
        int bufferSize = strlen(buffer);
        int bytesSent = sendto(sock, (const char*)buffer, 200, 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress));

        if (bytesSent == SOCKET_ERROR) {
            std::cout << "Error sending data " + std::to_string(WSAGetLastError()) << std::endl;
        }
    }

    ~Socket() {
        closesocket(sock);
    }
private:
    SOCKET sock = INVALID_SOCKET; 

    static constexpr std::string localhost = "127.0.0.1";
    static sockaddr_in formatAddress(const char* ip, unsigned short port)
    {
        sockaddr_in address;
        address.sin_family = AF_INET;
        InetPton(AF_INET, _T(ip), &address.sin_addr.s_addr);
        address.sin_port = htons(port);
        return address;
    }
};