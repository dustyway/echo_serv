/***************************************************************************
* A sample implementation of a concrete Reactor using the poll() function as
* a synchronous event demultiplexer.
****************************************************************************/

#include "PollReactor.h"
#include "Error.h"

#include <cassert>
#include <cstdio>
#include <cstring>

/* POSIX requires <poll.h> to define INFTIM.
   However, in my environment it doesn't -> follow the recommendations by Mr. Stevens.
*/
#ifndef INFTIM
#define INFTIM -1
#endif

/************************************************************
* PollReactor implementation
************************************************************/

PollReactor::PollReactor()
{
    for(int i = 0; i < MAX_NO_OF_HANDLES; ++i) {
        registeredHandlers_[i].isUsed = false;
        registeredHandlers_[i].handler = NULL;
        std::memset(&registeredHandlers_[i].fd, 0, sizeof(struct pollfd));
    }
}

PollReactor::~PollReactor()
{
    // Nothing to clean up
}

void PollReactor::registerHandler(EventHandler* handler)
{
    assert(NULL != handler);

    if(!addToRegistry(handler)) {
        error("No more registrations possible");
    }
}

void PollReactor::unregisterHandler(EventHandler* handler)
{
    assert(NULL != handler);

    if(!removeFromRegistry(handler)) {
        error("Event handler not registered");
    }
}

void PollReactor::handleEvents()
{
    struct pollfd fds[MAX_NO_OF_HANDLES];
    std::memset(fds, 0, sizeof(fds));

    const size_t noOfHandles = buildPollArray(fds);

    if(0 < poll(fds, noOfHandles, INFTIM)){
        dispatchSignalledHandles(fds, noOfHandles);
    }
    else{
        error("Poll failure");
    }
}

/************************************************************
* Private helper methods
************************************************************/

int PollReactor::addToRegistry(EventHandler* handler)
{
    int isRegistered = 0;

    for(int i = 0; (i < MAX_NO_OF_HANDLES) && (0 == isRegistered); ++i) {
        if(!registeredHandlers_[i].isUsed) {
            HandlerRegistration* freeEntry = &registeredHandlers_[i];

            freeEntry->handler = handler;
            freeEntry->fd.fd = handler->getHandle();
            freeEntry->fd.events = POLLRDNORM;
            freeEntry->isUsed = true;

            isRegistered = 1;

            std::printf("Reactor: Added handle with ID = %d\n", freeEntry->fd.fd);
        }
    }

    return isRegistered;
}

int PollReactor::removeFromRegistry(EventHandler* handler)
{
    int nodeRemoved = 0;

    for(int i = 0; (i < MAX_NO_OF_HANDLES) && (0 == nodeRemoved); ++i) {
        if(registeredHandlers_[i].isUsed && (registeredHandlers_[i].handler == handler)) {
            registeredHandlers_[i].isUsed = false;
            nodeRemoved = 1;

            std::printf("Reactor: Removed event handler with ID = %d\n",
                       registeredHandlers_[i].fd.fd);
        }
    }

    return nodeRemoved;
}

size_t PollReactor::buildPollArray(struct pollfd* fds)
{
    size_t noOfCopiedHandles = 0;

    for(int i = 0; i < MAX_NO_OF_HANDLES; ++i) {
        if(registeredHandlers_[i].isUsed) {
            fds[noOfCopiedHandles] = registeredHandlers_[i].fd;
            ++noOfCopiedHandles;
        }
    }

    return noOfCopiedHandles;
}

EventHandler* PollReactor::findHandler(int fd)
{
    EventHandler* matchingHandler = NULL;

    for(int i = 0; (i < MAX_NO_OF_HANDLES) && (NULL == matchingHandler); ++i) {
        if(registeredHandlers_[i].isUsed && (registeredHandlers_[i].fd.fd == fd)) {
            matchingHandler = registeredHandlers_[i].handler;
        }
    }

    return matchingHandler;
}

void PollReactor::dispatchSignalledHandles(const struct pollfd* fds, size_t noOfHandles)
{
    for(size_t i = 0; i < noOfHandles; ++i) {
        if((POLLRDNORM | POLLERR) & fds[i].revents) {
            EventHandler* signalledHandler = findHandler(fds[i].fd);

            if(NULL != signalledHandler){
                signalledHandler->handleEvent();
            }
        }
    }
}
