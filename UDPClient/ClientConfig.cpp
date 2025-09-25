#include "ClientConfig.h"
#include <string_view>
#include <filesystem>
#include <fstream>

namespace {
#ifdef _WIN32
    const std::string_view configPath = "C:/ProgramData/UDPClientServer/clientConfig.txt";
#else

#endif
}

ClientConfig::ClientConfig()
{
    std::ifstream f(std::filesystem::path{configPath});
    if (!f.is_open()) {
        throw std::runtime_error("No clientConfig.txt at executable's directory.");
    }
    std::string s;
    getline(f, s);

    static std::string addrPrefix = "[serverAddress] ";

    if (s.starts_with(addrPrefix)) {
        serverAddress = s.substr(addrPrefix.length(), s.length() - addrPrefix.length());
    } else {
        throw std::runtime_error("Config doesn't contain a valid serverAddress.");
    }

    getline(f, s);

    size_t nRead = sscanf(s.data(), "[serverPort] %d", &serverPort);

    if (nRead == 0)
        throw std::runtime_error("Config doesn't contain a valid serverPort.");

    getline(f, s);

    nRead = sscanf(s.data(), "[value] %lf", &value);

    if (nRead == 0)
        throw std::runtime_error("Config doesn't contain a valid value.");

    f.close();
}