#pragma once
#include <cstdint>

static constexpr size_t MTU = 1200;

#ifdef _WIN32
    constexpr std::string_view filesPath = "C:/ProgramData/UDPClientServer";
#else
    constexpr std::string_view filesPath = "/opt/UDPClientServer"
#endif