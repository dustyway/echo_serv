#include "TcpServer.h"
#include "Error.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

TcpServer::TcpServer(unsigned int tcpPort)
    : serverSocket_(-1)
{
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));

    serverSocket_ = socket(PF_INET, SOCK_STREAM, 0);

    if(serverSocket_ < 0) {
        error("Failed to create server socket");
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(tcpPort);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(serverSocket_, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) != 0) {
        error("bind() failed");
    }

    if(listen(serverSocket_, SOMAXCONN) != 0) {
        error("listen() failed");
    }
}

TcpServer::~TcpServer()
{
    if(serverSocket_ >= 0) {
        close(serverSocket_);
    }
}

int TcpServer::getSocket() const
{
    return serverSocket_;
}
