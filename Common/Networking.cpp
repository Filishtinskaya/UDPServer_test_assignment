#include "Networking.h"


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
#include <assert.h>

namespace {
    sockaddr_in formatAddress(const char* ip, unsigned short port)
    {
        sockaddr_in address;
        address.sin_family = AF_INET;
        InetPton(AF_INET, _T(ip), &address.sin_addr);
        address.sin_port = htons(port);
        return address;
    }
}

WSAHandler::WSAHandler() {
    WSAData wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0) {
        throw std::runtime_error("WSAStartup initialization error " + std::to_string(wsaerr));
    }
}

WSAHandler::~WSAHandler() {
    WSACleanup();
}

Socket::Socket(unsigned short port) {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("Error creating socket " + std::to_string(WSAGetLastError()));
    }

    sockaddr_in address = formatAddress(localhost.data(), port);

    if (bind(sock, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR) {
        throw std::runtime_error("Bind socket failed " + std::to_string(WSAGetLastError()));
    }
}

Socket::ReceivedMsg Socket::receive() const {
    ReceivedMsg res;
    res.message.resize(MTU);

    sockaddr_in senderAddress;
    int addrLength = sizeof(senderAddress);

    int bytes_received = recvfrom(sock, res.message.data(), res.message.size(), 0, (SOCKADDR*)&senderAddress, &addrLength);

    if (bytes_received == SOCKET_ERROR) {
        std::cout << "Error receiving data " + std::to_string(WSAGetLastError()) << std::endl;
        return {};
    }

    res.sourceIp.resize(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(senderAddress.sin_addr), res.sourceIp.data(), INET_ADDRSTRLEN);
    res.sourcePort = ntohs(senderAddress.sin_port);

    std::cout << "Received datagram: " << res.message << " from " << res.sourceIp << ":" << res.sourcePort << std::endl;

    return res;
}

void Socket::send(std::string ip, unsigned short port, std::string message) const {
    assert(message.size() <= MTU);

    sockaddr_in clientAddress = formatAddress(ip.data(), port);
    int bytesSent = sendto(sock, message.data(), message.size(), 0, (SOCKADDR*)&clientAddress, sizeof(clientAddress));

    if (bytesSent == SOCKET_ERROR) {
        std::cout << "Error sending data " + std::to_string(WSAGetLastError()) << std::endl;
    }
}

Socket::~Socket() {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}