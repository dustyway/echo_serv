#ifndef REACTOR_H
#define REACTOR_H

#include "EventHandler.h"

/**
 * Abstract base class for the Reactor pattern.
 * Concrete reactors (PollReactor, SelectReactor) inherit from this class
 * and implement the event demultiplexing strategy.
 */
class Reactor
{
public:
    virtual ~Reactor() {}

    /**
     * Register an event handler with the reactor.
     * @param handler Pointer to the event handler to register
     */
    virtual void registerHandler(EventHandler* handler) = 0;

    /**
     * Unregister an event handler from the reactor.
     * @param handler Pointer to the event handler to unregister
     */
    virtual void unregisterHandler(EventHandler* handler) = 0;

    /**
     * Process events by waiting for handles to become ready
     * and dispatching to their corresponding event handlers.
     */
    virtual void handleEvents() = 0;
};

#endif
