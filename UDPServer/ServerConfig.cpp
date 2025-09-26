#include "ServerConfig.h"
#include <string_view>
#include <fstream>
#include <filesystem>

#include "Utils.h"

namespace {
    constexpr std::string_view configName = "serverConfig.txt";
}

ServerConfig::ServerConfig()
{
    std::ifstream f(std::filesystem::path{filesPath} / configName);
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