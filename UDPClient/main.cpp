#include "ClientConfig.h"
#include "Networking.h"

int main()
{
    ClientConfig conf;

#ifdef _WIN32
    WSAHandler wsa;
#endif

    Socket sock(8081);

    while (true)
        sock.send(conf.getServerAddress(), conf.getServerPort());
    
    return 0;
}