#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <map>
#include <cstdio>
#include <cerrno>
#include <iostream>

#include "EventHandler.hpp"

class Reactor
{
public:
    static const int MAX_EVENTS = 64;

    Reactor();
    ~Reactor();

    bool init();
    void run();
    void stop();

    bool register_handler(EventHandler* handler, uint32_t events);
    bool modify_handler(EventHandler* handler, uint32_t events);
    void remove_handler(EventHandler* handler);

private:
    Reactor(const Reactor&);
    Reactor& operator=(const Reactor&);

    int _epollfd;
    bool _running;
    std::map<int, EventHandler*> _handlers;
};

#endif
