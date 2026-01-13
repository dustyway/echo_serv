#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdio>
#include <iostream>

#include "EventHandler.hpp"
#include "Reactor.hpp"

class SignalHandler : public EventHandler
{
public:
    SignalHandler(Reactor& reactor);
    virtual ~SignalHandler();

    bool open();

    virtual void handle_read();
    virtual void handle_write();
    virtual void handle_close();

    static int pipe_write_fd() { return _pipe[1]; }

private:
    SignalHandler(const SignalHandler&);
    SignalHandler& operator=(const SignalHandler&);

    static void signal_callback(int sig);
    static void set_nonblocking(int fd);

    static int _pipe[2];
    Reactor& _reactor;
};

#endif
