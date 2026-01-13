#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

class Reactor;

class EventHandler
{
public:
    EventHandler() : _fd(-1) {}
    virtual ~EventHandler() {}

    virtual void handle_read() = 0;
    virtual void handle_write() = 0;
    virtual void handle_close() = 0;

    int get_fd() const { return _fd; }

protected:
    int _fd;
};

#endif
