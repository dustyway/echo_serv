/***************************************************************************
* A sample implementation of a concrete Reactor using the select() function as
* a synchronous event demultiplexer.
****************************************************************************/

#include "SelectReactor.h"
#include "Error.h"

#include <cassert>
#include <cstdio>
#include <cstring>

/************************************************************
* SelectReactor implementation
************************************************************/

SelectReactor::SelectReactor()
{
    for(int i = 0; i < MAX_NO_OF_HANDLES; ++i) {
        registeredHandlers_[i].isUsed = false;
        registeredHandlers_[i].handler = NULL;
        registeredHandlers_[i].fd = -1;
    }
}

SelectReactor::~SelectReactor()
{
    // Nothing to clean up
}

void SelectReactor::registerHandler(EventHandler* handler)
{
    assert(NULL != handler);

    if(!addToRegistry(handler)) {
        error("No more registrations possible");
    }
}

void SelectReactor::unregisterHandler(EventHandler* handler)
{
    assert(NULL != handler);

    if(!removeFromRegistry(handler)) {
        error("Event handler not registered");
    }
}

void SelectReactor::handleEvents()
{
    fd_set readSet;
    FD_ZERO(&readSet);

    const int maxFd = buildSelectSet(&readSet);

    if(0 < select(maxFd + 1, &readSet, NULL, NULL, NULL)){
        dispatchSignalledHandles(&readSet);
    }
    else{
        error("Select failure");
    }
}

/************************************************************
* Private helper methods
************************************************************/

int SelectReactor::addToRegistry(EventHandler* handler)
{
    int isRegistered = 0;

    for(int i = 0; (i < MAX_NO_OF_HANDLES) && (0 == isRegistered); ++i) {
        if(!registeredHandlers_[i].isUsed) {
            HandlerRegistration* freeEntry = &registeredHandlers_[i];

            freeEntry->handler = handler;
            freeEntry->fd = handler->getHandle();
            freeEntry->isUsed = true;

            isRegistered = 1;

            std::printf("Reactor: Added handle with ID = %d\n", freeEntry->fd);
        }
    }

    return isRegistered;
}

int SelectReactor::removeFromRegistry(EventHandler* handler)
{
    int nodeRemoved = 0;

    for(int i = 0; (i < MAX_NO_OF_HANDLES) && (0 == nodeRemoved); ++i) {
        if(registeredHandlers_[i].isUsed && (registeredHandlers_[i].handler == handler)) {
            registeredHandlers_[i].isUsed = false;
            nodeRemoved = 1;

            std::printf("Reactor: Removed event handler with ID = %d\n",
                       registeredHandlers_[i].fd);
        }
    }

    return nodeRemoved;
}

int SelectReactor::buildSelectSet(fd_set* readSet)
{
    int maxFd = -1;

    for(int i = 0; i < MAX_NO_OF_HANDLES; ++i) {
        if(registeredHandlers_[i].isUsed) {
            FD_SET(registeredHandlers_[i].fd, readSet);

            if(registeredHandlers_[i].fd > maxFd) {
                maxFd = registeredHandlers_[i].fd;
            }
        }
    }

    return maxFd;
}

EventHandler* SelectReactor::findHandler(int fd)
{
    EventHandler* matchingHandler = NULL;

    for(int i = 0; (i < MAX_NO_OF_HANDLES) && (NULL == matchingHandler); ++i) {
        if(registeredHandlers_[i].isUsed && (registeredHandlers_[i].fd == fd)) {
            matchingHandler = registeredHandlers_[i].handler;
        }
    }

    return matchingHandler;
}

void SelectReactor::dispatchSignalledHandles(const fd_set* readSet)
{
    for(int i = 0; i < MAX_NO_OF_HANDLES; ++i) {
        if(registeredHandlers_[i].isUsed &&
           FD_ISSET(registeredHandlers_[i].fd, readSet)) {
            EventHandler* signalledHandler = registeredHandlers_[i].handler;

            if(NULL != signalledHandler){
                signalledHandler->handleEvent();
            }
        }
    }
}
