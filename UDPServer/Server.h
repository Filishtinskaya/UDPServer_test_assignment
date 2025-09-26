#pragma once

#include "ServerConfig.h"

#include <iostream>

#include "Networking.h"
#include "Protocol.h"
#include <thread>
#include <cstdint>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <random>
#include <optional>

constexpr size_t TOTAL_VALUES = 1'000'000;

class Server
{
public:
    Server() : listeningSocket(conf.getPort()), 
               sendingSocket(conf.getPort() + 10), 
               listenThread(&Server::listen, this), 
               sendThread(&Server::send, this),
               processThread(&Server::process, this)
    {
        
    }
private:
    class MessageQueue
    {
    public:
        void add(Message&& msg) 
        {
            {
                std::scoped_lock lk(mtx);
                queue.emplace(std::move(msg));
            }
            cv.notify_one();
        }
        Message blockingGet() 
        {
            std::unique_lock lk(mtx);
            cv.wait(lk, [&](){return !queue.empty();});
            Message res = std::move(queue.front());
            queue.pop();
            return res;
        }
        std::optional<Message> nonblockingGet()
        {
            std::scoped_lock lk(mtx);
            if (queue.empty())
                return {};
            std::optional<Message> res = queue.front();
            queue.pop();
            return res;
        }
    private:
        std::mutex mtx;
        std::condition_variable cv;
        std::queue<Message> queue;
    };

    struct Connection
    {
        std::string ip;
        unsigned short port;
        double limit;
        size_t packetsSent;
    };

    MessageQueue received, toSend;

    ServerConfig conf;
    Socket listeningSocket;
    Socket sendingSocket;

    std::jthread listenThread, sendThread, processThread;

    std::vector<Connection> connections;

    void listen() {
        while(true) {
            Message msg = listeningSocket.receive();
            received.add(std::move(msg));
        }
    }

    void send() {
        while(true) {
            Message msg = toSend.blockingGet();
            sendingSocket.send(msg);
        }
    }

    void process() {
        while (true) {
            /// first answer first received message to ensure responsiveness, then send a data packet to each of the existing connections in a row
            processReceived();
            processConnections();
        }
    }

    void processReceived() {
        auto msgOpt = received.nonblockingGet();
        if (msgOpt.has_value()) {
            switch(msgOpt->packet.basic.header.type) {
                case PacketType::Connect: {
                    Packet pck;
                    pck.basic.header.protocolVersion = CUR_PROTOCOL_VERSION;
                    pck.basic.header.type = PacketType::Connect;
                    toSend.add(Message{msgOpt->ip, msgOpt->port, pck});
                    break;
                }
                case PacketType::Request: {
                    connections.emplace_back(msgOpt->ip, msgOpt->port, msgOpt->packet.request.value, 0);
                    break;
                }
                case PacketType::Data:
                case PacketType::Fin: {
                    throw std::logic_error ("Server received the packet of a wrong type");
                }
            }
        }
    } 

    void processConnections() {
        for (auto& conn : connections) {
            Packet packet;
            packet.data.header.protocolVersion = 1;
            packet.data.header.type = PacketType::Data;

            std::array<double, VALUES_IN_DATA_PACKET> arr;
            std::uniform_real_distribution<double> urd(-conn.limit, conn.limit);
            std::default_random_engine re;
            for (size_t i = 0; i < VALUES_IN_DATA_PACKET; i++) {
                packet.data.data[i] = urd(re);
            }
            toSend.add({conn.ip, conn.port, packet});

            conn.packetsSent++;
            if (conn.packetsSent == TOTAL_VALUES / VALUES_IN_DATA_PACKET) {
                Packet pck;
                pck.basic.header.protocolVersion = CUR_PROTOCOL_VERSION;
                pck.basic.header.type = PacketType::Fin;
                toSend.add(Message{conn.ip, conn.port, pck});
            }
        }
        std::erase_if(connections, [](const Connection& conn){
            return conn.packetsSent == TOTAL_VALUES / VALUES_IN_DATA_PACKET;
        });
    }
};