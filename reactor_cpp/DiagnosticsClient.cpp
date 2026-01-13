/*********************************************************************************
*
* The server side representation of a client sending diagnostic messages to the log.
*
* An instance of this client is created as the server detects a connect request.
* Upon creation, the instance registers itself at the Reactor. The Reactor will notify
* this client representation as an incoming diagnostics message is pending.
* This client representation simply reads the message and prints it to stdout.
**********************************************************************************/

#include "DiagnosticsClient.h"
#include "Error.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <cassert>

#define MAX_MESSAGE_SIZE 1024

/************************************************************
* Local helper function
************************************************************/

static Handle acceptClientConnection(int serverHandle)
{
    struct sockaddr_in clientAddress;
    std::memset(&clientAddress, 0, sizeof(clientAddress));

    socklen_t addressSize = sizeof(clientAddress);

    const Handle clientHandle = accept(serverHandle,
                                      reinterpret_cast<struct sockaddr*>(&clientAddress),
                                      &addressSize);

    if(0 > clientHandle) {
        error("Failed to accept client connection");
    }

    std::printf("Client: New connection created on IP-address %X\n",
                ntohl(clientAddress.sin_addr.s_addr));

    return clientHandle;
}

/************************************************************
* DiagnosticsClient implementation
************************************************************/

DiagnosticsClient::DiagnosticsClient(Handle serverHandle, ServerEventNotifier* notifier)
    : clientSocket_(-1), notifier_(notifier)
{
    assert(notifier != NULL);
    clientSocket_ = acceptClientConnection(serverHandle);
}

DiagnosticsClient::~DiagnosticsClient()
{
    if(clientSocket_ >= 0) {
        close(clientSocket_);
    }
}

Handle DiagnosticsClient::getHandle() const
{
    return clientSocket_;
}

void DiagnosticsClient::handleEvent()
{
    char diagnosticMessage[MAX_MESSAGE_SIZE];
    std::memset(diagnosticMessage, 0, sizeof(diagnosticMessage));

    const ssize_t receiveResult = recv(clientSocket_, diagnosticMessage,
                                      sizeof(diagnosticMessage), 0);

    if(0 < receiveResult) {
        /* In the real world, we would probably put a protocol on top of TCP/IP in
        order to be able to restore the message out of the byte stream (it is no
        guarantee that the complete message is received in one recv(). */

        std::printf("Client: Diagnostics received - %s\n", diagnosticMessage);
    }
    else {
        // Connection closed or error - notify the server
        notifier_->onClientClosed(this);
    }
}
