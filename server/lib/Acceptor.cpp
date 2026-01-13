#include "Acceptor.hpp"

Acceptor::Acceptor(Reactor& reactor, const std::string& port)
    : _reactor(reactor)
    , _port(port)
{
}

Acceptor::~Acceptor()
{
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }
}

void Acceptor::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool Acceptor::open()
{
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        perror("socket");
        return false;
    }

    int optval = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        return false;
    }

    set_nonblocking(_fd);

    sockaddr_in servaddr = {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(_port.c_str()));

    if (bind(_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        return false;
    }

    if (listen(_fd, SOMAXCONN) < 0) {
        perror("listen");
        return false;
    }

    if (!_reactor.register_handler(this, EPOLLIN)) {
        return false;
    }

    std::cout << "Server listening on port " << _port << std::endl;
    return true;
}

void Acceptor::handle_read()
{
    // Accept all pending connections
    while (true) {
        sockaddr_in cliaddr = {};
        socklen_t clilen = sizeof(cliaddr);

        int connfd = accept(_fd, (struct sockaddr*)&cliaddr, &clilen);
        if (connfd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  // No more pending connections
            }
            perror("accept");
            break;
        }

        set_nonblocking(connfd);

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "New connection: fd=" << connfd 
                  << " from " << client_ip << ":" << ntohs(cliaddr.sin_port)
                  << std::endl;

        ConnectionHandler* handler = new ConnectionHandler(connfd, _reactor);
        if (!_reactor.register_handler(handler, EPOLLIN)) {
            delete handler;
        }
    }
}

void Acceptor::handle_write()
{
    // Acceptor doesn't write
}

void Acceptor::handle_close()
{
    std::cout << "Acceptor closing" << std::endl;
    _reactor.remove_handler(this);
}
