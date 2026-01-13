#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

#include "Reactor.hpp"
#include "Acceptor.hpp"
#include "SignalHandler.hpp"

class Server
{
public:
    Server(const std::string& port);
    ~Server();

    int run();

private:
    Server(const Server&);
    Server& operator=(const Server&);

    std::string     _port;
    Reactor         _reactor;
    SignalHandler*  _signal_handler;
    Acceptor*       _acceptor;
};

#endif