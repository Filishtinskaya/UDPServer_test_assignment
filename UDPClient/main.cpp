#include "ClientConfig.h"
#include "Networking.h"

int main()
{
    ClientConfig conf;

#ifdef _WIN32
    WSAHandler wsa;
#endif

    Socket sock(8081);

    for (size_t i = 0; i < 20; i++)
        sock.send(conf.getServerAddress(), conf.getServerPort(), "test2");
    
    return 0;
}