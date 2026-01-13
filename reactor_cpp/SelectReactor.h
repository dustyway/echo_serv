#ifndef SELECT_REACTOR_H
#define SELECT_REACTOR_H

#include "Reactor.h"
#include <sys/select.h>
#include <cstddef>

#define MAX_NO_OF_HANDLES 32

/**
 * Concrete implementation of Reactor using select() as the event demultiplexer.
 */
class SelectReactor : public Reactor
{
public:
    SelectReactor();
    virtual ~SelectReactor();

    virtual void registerHandler(EventHandler* handler);
    virtual void unregisterHandler(EventHandler* handler);
    virtual void handleEvents();

private:
    struct HandlerRegistration
    {
        bool isUsed;
        EventHandler* handler;
        Handle fd;
    };

    HandlerRegistration registeredHandlers_[MAX_NO_OF_HANDLES];

    int addToRegistry(EventHandler* handler);
    int removeFromRegistry(EventHandler* handler);
    int buildSelectSet(fd_set* readSet);
    EventHandler* findHandler(int fd);
    void dispatchSignalledHandles(const fd_set* readSet);

    // Prevent copying
    SelectReactor(const SelectReactor&);
    SelectReactor& operator=(const SelectReactor&);
};

#endif
