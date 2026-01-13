#ifndef POLL_REACTOR_H
#define POLL_REACTOR_H

#include "Reactor.h"
#include <poll.h>
#include <cstddef>

#define MAX_NO_OF_HANDLES 32

/**
 * Concrete implementation of Reactor using poll() as the event demultiplexer.
 */
class PollReactor : public Reactor
{
public:
    PollReactor();
    virtual ~PollReactor();

    virtual void registerHandler(EventHandler* handler);
    virtual void unregisterHandler(EventHandler* handler);
    virtual void handleEvents();

private:
    struct HandlerRegistration
    {
        bool isUsed;
        EventHandler* handler;
        struct pollfd fd;
    };

    HandlerRegistration registeredHandlers_[MAX_NO_OF_HANDLES];

    int addToRegistry(EventHandler* handler);
    int removeFromRegistry(EventHandler* handler);
    size_t buildPollArray(struct pollfd* fds);
    EventHandler* findHandler(int fd);
    void dispatchSignalledHandles(const struct pollfd* fds, size_t noOfHandles);

    // Prevent copying
    PollReactor(const PollReactor&);
    PollReactor& operator=(const PollReactor&);
};

#endif
