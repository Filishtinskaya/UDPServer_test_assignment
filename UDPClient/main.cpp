#include "ClientConfig.h"
#include "Networking.h"
#include "Protocol.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <algorithm>

constexpr std::string_view logName = "clientLog.txt";

void connect(std::string_view ip, unsigned short port, Socket& sock) {
    Message msg;
    msg.ip = ip;
    msg.port = port;
    msg.packet.basic.header.protocolVersion = CUR_PROTOCOL_VERSION;
    msg.packet.basic.header.type = PacketType::Connect;
    sock.send(std::move(msg));

    Message response = sock.receive();
    
    if (response.empty()) {
        throw std::runtime_error("Could not connect to the server.");
    }
        
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
    sock.send(std::move(msg));
}

void receiveData(Socket& sock, unsigned short id) {
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

    for (size_t i = 0; i < VALUES_IN_DATA_PACKET; i++) {
        std::cout << res[i] << " ";
    }

    std::sort(res.begin(), res.end());

    auto time = std::time(nullptr);
    std::string resFileName = "res at " + std::to_string(id) + " ";
    std::string timeStr;
    timeStr.resize(20);
    strftime( timeStr.data(), 20, "%F %H-%M-%S", std::localtime(&time) );
    resFileName.append(timeStr);
    
    std::ofstream resFile(std::filesystem::path{filesPath} / resFileName, std::ios::binary);
    if (!resFile.is_open()) {
        throw std::runtime_error("Error opening result file.");
    }

    resFile.write(reinterpret_cast<const char*>(res.data()), res.size() * sizeof(double));
    
    resFile.close();
}

int main(int argc, char* argv[])
{
    if (!std::numeric_limits<double>::is_iec559) {
        throw std::logic_error("This platform does not have IEEE-754 compliant doubles, so sending them over the network is unreliable.");
    }
    
    ClientConfig conf;

#ifdef _WIN32
    WSAHandler wsa;
#endif
    unsigned short ownPort = 9090;

    if (argc == 2)
        ownPort = std::stoi(argv[1]);
        
    Socket sock(ownPort);

    auto ip = conf.getServerAddress();
    unsigned short port = conf.getServerPort();

    std::thread t([&](){
        connect(ip, port, sock);
        std::this_thread::sleep_for(std::chrono::seconds{3});
        sendRequest(ip, port, sock, conf.getValue());
        receiveData(sock, ownPort);
    });

    t.detach();
    std::cout << "Client is running, press Enter to exit";

    getchar();
    
    return 0;
}