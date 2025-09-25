#pragma once

class ServerConfig
{
public:
    ServerConfig();
    unsigned short getPort() const noexcept {
        return port;
    }
private:
    unsigned short port;
};