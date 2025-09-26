#pragma once
#include <string>

class ClientConfig
{
public:
    ClientConfig();
    std::string_view getServerAddress() const noexcept {
        return serverAddress;
    }
    unsigned short getServerPort() const noexcept {
        return serverPort;
    }
    double getValue() const noexcept {
        return value;
    }
private:
    std::string serverAddress;
    unsigned short serverPort;
    double value;
};