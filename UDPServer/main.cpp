#include "ServerConfig.h"
#include "Networking.h"
#include <iostream>

int main()
{
    ServerConfig conf;

#ifdef _WIN32
    WSAHandler wsa;
#endif

    Socket sock(conf.getPort());

    while(true) {
        auto msg = sock.receive();
    }
        
    
    return 0;
}