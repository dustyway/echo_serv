#ifndef CONNECTIONHANDLER_HPP
#define CONNECTIONHANDLER_HPP

#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <string>

#include "EventHandler.hpp"
#include "Reactor.hpp"

class ConnectionHandler : public EventHandler
{
public:
    static const size_t BUFFER_SIZE = 4096;

    ConnectionHandler(int fd, Reactor& reactor);
    virtual ~ConnectionHandler();

    virtual void handle_read();
    virtual void handle_write();
    virtual void handle_close();

private:
    ConnectionHandler(const ConnectionHandler&);
    ConnectionHandler& operator=(const ConnectionHandler&);

    Reactor& _reactor;
    std::string _read_buffer;
    std::string _write_buffer;
};

#endif
