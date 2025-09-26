#include <iostream>
#include "Networking.h"
#include "Server.h"

int main()
{
    if (!std::numeric_limits<double>::is_iec559) {
        throw std::logic_error("This platform does not have IEEE-754 compliant doubles, so sending them over the network is unreliable.");
    }

#ifdef _WIN32
    WSAHandler wsa;
#endif
    Server sv;

    std::cout << "Server is running, press Enter to exit";

    getchar();
        
    return 0;
}