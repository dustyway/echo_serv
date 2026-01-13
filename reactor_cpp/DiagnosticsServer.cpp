/******************************************************************************
* A simulation of a diagnostics server.
*
* A client that wants to log a diagnostic connects to this server using TCP/IP.
* The server gets notified about a connecting client through the Reactor.
* Upon such a notification, the server creates a client representation.
*******************************************************************************/

#include "DiagnosticsServer.h"
#include "DiagnosticsClient.h"
#include "Reactor.h"
#include "Error.h"

#include <cstdio>
#include <cassert>

/************************************************************
* DiagnosticsServer implementation
************************************************************/

DiagnosticsServer::DiagnosticsServer(unsigned int tcpPort, Reactor* reactor)
    : tcpServer_(tcpPort), reactor_(reactor)
{
    assert(reactor != NULL);

    for(int i = 0; i < MAX_NO_OF_CLIENTS; ++i) {
        clients_[i] = NULL;
    }

    // Register this server with the reactor to listen for connections
    reactor_->registerHandler(this);
}

DiagnosticsServer::~DiagnosticsServer()
{
    // Unregister from reactor first
    reactor_->unregisterHandler(this);

    // Clean up all clients
    deleteAllClients();
}

Handle DiagnosticsServer::getHandle() const
{
    return tcpServer_.getSocket();
}

void DiagnosticsServer::handleEvent()
{
    const int freeSlot = findFreeClientSlot();

    if(0 <= freeSlot) {
        // Create a new client and register it with the reactor
        DiagnosticsClient* newClient = new DiagnosticsClient(tcpServer_.getSocket(), this);
        clients_[freeSlot] = newClient;

        reactor_->registerHandler(newClient);

        std::printf("Server: Incoming connect request accepted\n");
    }
    else {
        std::printf("Server: No space for more clients\n");
    }
}

void DiagnosticsServer::onClientClosed(DiagnosticsClient* client)
{
    const int clientSlot = findClientSlot(client);

    if(0 > clientSlot) {
        error("Phantom client detected");
    }

    // Unregister client from reactor
    reactor_->unregisterHandler(client);

    // Delete the client
    delete client;

    clients_[clientSlot] = NULL;

    std::printf("Server: Client connection closed\n");
}

/************************************************************
* Private helper methods
************************************************************/

int DiagnosticsServer::findFreeClientSlot() const
{
    return findClientSlot(NULL);
}

int DiagnosticsServer::findClientSlot(DiagnosticsClient* client) const
{
    int clientSlot = -1;
    int slotFound = 0;

    for(int i = 0; (i < MAX_NO_OF_CLIENTS) && (0 == slotFound); ++i) {
        if(client == clients_[i]) {
            clientSlot = i;
            slotFound = 1;
        }
    }

    return clientSlot;
}

void DiagnosticsServer::deleteAllClients()
{
    for(int i = 0; i < MAX_NO_OF_CLIENTS; ++i) {
        if(NULL != clients_[i]) {
            reactor_->unregisterHandler(clients_[i]);
            delete clients_[i];
            clients_[i] = NULL;
        }
    }
}
