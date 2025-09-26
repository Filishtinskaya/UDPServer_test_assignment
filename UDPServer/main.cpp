#include <iostream>
#include "Networking.h"
#include "Server.h"

int main()
{
#ifdef _WIN32
    WSAHandler wsa;
#endif
    Server sv;

    std::cout << "Server is running, press any key to exit";

    getchar();
        
    return 0;
}