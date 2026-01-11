#ifndef DIAGNOSTICS_CLIENT_H
#define DIAGNOSTICS_CLIENT_H

#include "EventHandler.h"
#include "ServerEventNotifier.h"

/**
 * Represents a client connection that sends diagnostic messages.
 * Inherits from EventHandler to receive notifications from the Reactor.
 */
class DiagnosticsClient : public EventHandler
{
public:
    /**
     * Create a client representation for an incoming connection.
     * @param serverHandle Server socket with pending connection
     * @param notifier Interface for notifying the server of events
     */
    DiagnosticsClient(Handle serverHandle, ServerEventNotifier* notifier);

    /**
     * Destructor closes the client socket.
     */
    virtual ~DiagnosticsClient();

    /**
     * Get the client socket handle.
     * @return The client socket file descriptor
     */
    virtual Handle getHandle() const;

    /**
     * Handle incoming data from the client.
     * Called by the Reactor when data is ready to read.
     */
    virtual void handleEvent();

private:
    Handle clientSocket_;
    ServerEventNotifier* notifier_;

    // Prevent copying
    DiagnosticsClient(const DiagnosticsClient&);
    DiagnosticsClient& operator=(const DiagnosticsClient&);
};

#endif
