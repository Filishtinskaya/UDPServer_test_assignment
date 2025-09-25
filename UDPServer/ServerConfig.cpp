#include "ServerConfig.h"
#include <string_view>
#include <filesystem>
#include <fstream>

#include <iostream>

namespace {
#ifdef _WIN32
    const std::string_view configPath = "C:/ProgramData/UDPClientServer/serverConfig.txt";
#else

#endif
}

ServerConfig::ServerConfig()
{
    std::ifstream f(std::filesystem::path{configPath});
    if (!f.is_open()) {
        throw std::runtime_error("No serverConfig.txt at executable's directory.");
    }
    std::string s;
    getline(f, s);
    f.close();

    int nRead = sscanf(s.data(), "[port] %d", &port);

    if (nRead == 0)
        throw std::runtime_error("Config doesn't contain a valid port.");
}