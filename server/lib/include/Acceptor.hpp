#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "EventHandler.hpp"
#include "Reactor.hpp"
#include "ConnectionHandler.hpp"

class Acceptor : public EventHandler
{
public:
    Acceptor(Reactor& reactor, const std::string& port);
    virtual ~Acceptor();

    bool open();

    virtual void handle_read();
    virtual void handle_write();
    virtual void handle_close();

private:
    Acceptor(const Acceptor&);
    Acceptor& operator=(const Acceptor&);

    static void set_nonblocking(int fd);

    Reactor& _reactor;
    std::string _port;
};

#endif
