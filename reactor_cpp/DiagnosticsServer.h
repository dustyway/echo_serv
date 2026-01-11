#ifndef DIAGNOSTICS_SERVER_H
#define DIAGNOSTICS_SERVER_H

#include "EventHandler.h"
#include "ServerEventNotifier.h"
#include "TcpServer.h"

class DiagnosticsClient;
class Reactor;

#define MAX_NO_OF_CLIENTS 10

/**
 * DiagnosticsServer manages client connections and handles incoming connect requests.
 * Inherits from EventHandler to receive connection notifications from the Reactor.
 * Implements ServerEventNotifier to handle callbacks from clients.
 */
class DiagnosticsServer : public EventHandler, public ServerEventNotifier
{
public:
    /**
     * Create a diagnostics server listening on the specified port.
     * @param tcpPort Port number to listen on
     * @param reactor Reactor instance for event registration
     */
    DiagnosticsServer(unsigned int tcpPort, Reactor* reactor);

    /**
     * Destructor cleans up all client connections.
     */
    virtual ~DiagnosticsServer();

    /**
     * Get the listening socket handle.
     * @return The server socket file descriptor
     */
    virtual Handle getHandle() const;

    /**
     * Handle incoming connection requests.
     * Called by the Reactor when a new connection is pending.
     */
    virtual void handleEvent();

    /**
     * Called by a client when its connection is closed.
     * @param client Pointer to the closed client
     */
    virtual void onClientClosed(DiagnosticsClient* client);

private:
    TcpServer tcpServer_;
    Reactor* reactor_;
    DiagnosticsClient* clients_[MAX_NO_OF_CLIENTS];

    int findFreeClientSlot() const;
    int findClientSlot(DiagnosticsClient* client) const;
    void deleteAllClients();

    // Prevent copying
    DiagnosticsServer(const DiagnosticsServer&);
    DiagnosticsServer& operator=(const DiagnosticsServer&);
};

#endif
