#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

typedef int Handle;

/**
 * Abstract base class for event handlers in the Reactor pattern.
 * Concrete event handlers must inherit from this class and implement
 * the pure virtual functions.
 */
class EventHandler
{
public:
    virtual ~EventHandler() {}

    /**
     * Get the handle (file descriptor) associated with this event handler.
     * @return The handle to be monitored by the Reactor
     */
    virtual Handle getHandle() const = 0;

    /**
     * Handle the event when the associated handle becomes ready.
     * This method is called by the Reactor when an event is detected.
     */
    virtual void handleEvent() = 0;
};

#endif
