#include "ClientConfig.h"
#include "Networking.h"
#include "Protocol.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <algorithm>

constexpr std::string_view logName = "clientLog.txt";
constexpr std::string_view resFileName = "resFile";

void connect(std::string_view ip, unsigned short port, Socket& sock) {
    Message msg;
    msg.ip = ip;
    msg.port = port;
    msg.packet.basic.header.protocolVersion = CUR_PROTOCOL_VERSION;
    msg.packet.basic.header.type = PacketType::Connect;
    sock.send(msg);

    Message response = sock.receive();
        
    if (response.packet.basic.header.type != PacketType::Connect) {
        throw std::runtime_error("Expected connect packet, received something else.");
    }

    if (response.packet.basic.header.protocolVersion != CUR_PROTOCOL_VERSION) {
        std::ofstream f(std::filesystem::path{filesPath} / logName);
        f << "Protocol version mismatch: client " << std::to_string(CUR_PROTOCOL_VERSION) << ", server " << response.packet.basic.header.protocolVersion;
        f.close();
        throw std::runtime_error("Protocol version mismatch.");
    }
}

void sendRequest(std::string_view ip, unsigned short port, Socket& sock, double value) {
    Message msg;
    msg.ip = ip;
    msg.port = port;
    msg.packet.request.header.protocolVersion = CUR_PROTOCOL_VERSION;
    msg.packet.request.header.type = PacketType::Request;
    msg.packet.request.value = value;
    sock.send(msg);
}

void receiveData(Socket& sock) {
    std::vector<double> res;
    res.reserve(5'000'000);
    while(true) {
        Message response = sock.receive();
        if (response.packet.basic.header.type == PacketType::Fin) {
            break;
        }

        if (response.packet.basic.header.type != PacketType::Data) {
            throw std::runtime_error("Wrong packet type.");
        }
        std::copy(response.packet.data.data.begin(), response.packet.data.data.end(), std::back_inserter(res));
    }

    std::sort(res.begin(), res.end());

    std::ofstream resFile(std::filesystem::path{filesPath} / resFileName, std::ios::binary);
    for (size_t i = 0; i < res.size(); i++) {
        resFile << res[i];
    }
    
    resFile.close();
}

int main()
{
    ClientConfig conf;

#ifdef _WIN32
    WSAHandler wsa;
#endif

    Socket sock(8081);

    auto ip = conf.getServerAddress();
    unsigned short port = conf.getServerPort();

    std::thread t([&](){
        connect(ip, port, sock);
        std::this_thread::sleep_for(std::chrono::seconds{3});
        sendRequest(ip, port, sock, conf.getValue());
        receiveData(sock);
    });

    t.detach();
    std::cout << "Client is running, press any key to exit";

    getchar();
    
    return 0;
}