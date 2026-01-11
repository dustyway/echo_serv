#ifndef SERVER_EVENT_NOTIFIER_H
#define SERVER_EVENT_NOTIFIER_H

class DiagnosticsClient;

/**
 * Interface for server event notifications.
 * Allows clients to notify their server about important events.
 */
class ServerEventNotifier
{
public:
    virtual ~ServerEventNotifier() {}

    /**
     * Called when a client connection is closed.
     * @param client Pointer to the client that was closed
     */
    virtual void onClientClosed(DiagnosticsClient* client) = 0;
};

#endif
