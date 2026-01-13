#include "Reactor.hpp"

Reactor::Reactor()
    : _epollfd(-1)
    , _running(false)
{
}

Reactor::~Reactor()
{
    if (_epollfd >= 0) {
        close(_epollfd);
        _epollfd = -1;
    }
}

bool Reactor::init()
{
    _epollfd = epoll_create1(EPOLL_CLOEXEC);
    if (_epollfd < 0) {
        perror("epoll_create1");
        return false;
    }
    return true;
}

void Reactor::run()
{
    _running = true;
    epoll_event events[MAX_EVENTS];

    while (_running) {
        int nready = epoll_wait(_epollfd, events, MAX_EVENTS, -1);

        if (nready < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nready; i++) {
            int fd = events[i].data.fd;
            uint32_t revents = events[i].events;

            std::map<int, EventHandler*>::iterator it = _handlers.find(fd);
            if (it == _handlers.end()) {
                continue;
            }

            EventHandler* handler = it->second;

            if (revents & (EPOLLERR | EPOLLHUP)) {
                handler->handle_close();
            }
            else {
                if (revents & EPOLLIN) {
                    handler->handle_read();
                }
                if (revents & EPOLLOUT) {
                    handler->handle_write();
                }
            }
        }
    }
}

void Reactor::stop()
{
    _running = false;
}

bool Reactor::register_handler(EventHandler* handler, uint32_t events)
{
    int fd = handler->get_fd();
    
    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl ADD");
        return false;
    }

    _handlers[fd] = handler;
    return true;
}

bool Reactor::modify_handler(EventHandler* handler, uint32_t events)
{
    int fd = handler->get_fd();

    epoll_event ev = {};
    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
        perror("epoll_ctl MOD");
        return false;
    }

    return true;
}

void Reactor::remove_handler(EventHandler* handler)
{
    int fd = handler->get_fd();
    
    epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL);
    _handlers.erase(fd);
}
