#include "Networking.h"

#include <stdexcept>
#include <tchar.h>
#include <iostream>
#include <assert.h>


#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif

#include "Utils.h"

namespace {
    sockaddr_in formatAddress(const char* ip, unsigned short port)
    {
        sockaddr_in address;
        address.sin_family = AF_INET;
        InetPton(AF_INET, _T(ip), &address.sin_addr);
        address.sin_port = htons(port);
        return address;
    }

    void reverseDoubleByteOrder(double& n)
    {
        uint32_t* h =  reinterpret_cast<uint32_t*>(&n);
        uint32_t* l =  h + 1;
        uint32_t tmp = *h;
        *h = htonl(*l);
        *l = htonl(tmp);
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

void Message::networkEndianConversion() {
    if constexpr (std::endian::native == std::endian::big)
        return;
    switch (packet.basic.header.type) {
        case PacketType::Connect:
        case PacketType::Fin: {
            return;
        }
        case PacketType::Request: {
            reverseDoubleByteOrder(packet.request.value);
            break;
        }
        case PacketType::Data: {
            for (size_t i = 0; i < packet.data.data.size(); i++) {
                reverseDoubleByteOrder(packet.data.data[i]);
            }
        }
    }
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

Message Socket::receive() const {
    Message res;

    sockaddr_in senderAddress;
    int addrLength = sizeof(senderAddress);

    int bytes_received = recvfrom(sock, res.packet.msg, MTU, 0, (SOCKADDR*)&senderAddress, &addrLength);

    if (bytes_received == SOCKET_ERROR) {
        std::cout << "Error receiving data " + std::to_string(WSAGetLastError()) << std::endl;
        return {};
    }

    res.ip.resize(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(senderAddress.sin_addr), res.ip.data(), INET_ADDRSTRLEN);
    res.port = ntohs(senderAddress.sin_port);

    res.networkEndianConversion();
    return res;
}

void Socket::send(Message&& message) const {
    message.networkEndianConversion();
    sockaddr_in clientAddress = formatAddress(message.ip.data(), message.port);
    int bytesSent = sendto(sock, 
        message.packet.msg, 
        packetLength(message.packet.basic.header.type), 
        0, 
        (SOCKADDR*)&clientAddress, 
        sizeof(clientAddress));

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