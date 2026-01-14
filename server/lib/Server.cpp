#include "Server.hpp"

Server::Server(const std::string& port)
    : _signal_handler(NULL)
    , _acceptor(new Acceptor(_reactor, port))
{
}

Server::~Server()
{
    delete _acceptor;
    delete _signal_handler;
}

int Server::run()
{
    if (!_reactor.init()) {
        return 1;
    }

    _signal_handler = new SignalHandler(_reactor);
    if (!_signal_handler->open()) {
        return 1;
    }

    if (!_acceptor->open()) {
        return 1;
    }

    std::cout << "Echo server started" << std::endl;
    _reactor.run();

    std::cout << "Server shutdown complete" << std::endl;
    return 0;
}
