#include "ServerConfig.h"
#include "Networking.h"

int main()
{
    ServerConfig conf;

#ifdef _WIN32
    WSAHandler wsa;
#endif

    Socket sock(conf.getPort());

    while(true)
        sock.receive();
    
    return 0;
}