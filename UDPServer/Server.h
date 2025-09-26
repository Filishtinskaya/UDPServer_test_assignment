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
    Server();
private:
    class MessageQueue
    {
    public:
        void add(Message&& msg);
        Message blockingGet();
        std::optional<Message> nonblockingGet();
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

    std::thread listenThread, sendThread, processThread;

    std::vector<Connection> connections;

    void listen();
    void send();
    void process();

    void processReceived();
    void processConnections();
};