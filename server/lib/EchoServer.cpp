#include "EchoServer.hpp"


int EchoServer::_signal_pipe[2] = {-1, -1};


void EchoServer::signal_handler(int sig)
{
    int saved_errno = errno;
    write(_signal_pipe[1], "x", 1);
    errno = saved_errno;
}

static void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

EchoServer::EchoServer(const std::string &port)
    : _is_running(false)
    , _port(port)
    , _listenfd(-1)
    , _epollfd(-1)
{
}

EchoServer::~EchoServer()
{
    std::cout << "\nCleaning up..." << std::endl;
    std::for_each(_clients.begin(), _clients.end(), close_if_valid);
    _clients.clear();

    if (_listenfd >= 0) {
        close(_listenfd);
        _listenfd = -1;
    }

    if (_epollfd >= 0) {
        close(_epollfd);
        _epollfd = -1;
    }

    if (_signal_pipe[0] >= 0) {
        close(_signal_pipe[0]);
        _signal_pipe[0] = -1;
    }
    if (_signal_pipe[1] >= 0) {
        close(_signal_pipe[1]);
        _signal_pipe[1] = -1;
    }

    std::cout << "Server shutdown complete" << std::endl;
}


void EchoServer::add_client(SOCKET fd)
{
    if (_clients.size() < MAX_CLIENTS) {
        _clients.push_back(fd);
    }
}

void EchoServer::remove_client(SOCKET fd)
{
    _clients.erase(std::remove(_clients.begin(), _clients.end(), fd), _clients.end());
}

void EchoServer::close_if_valid(SOCKET fd)
{
    if (fd >= 0) {
        close(fd);
    }
}

bool EchoServer::init()
{
    if (pipe(_signal_pipe) < 0) {
        perror("pipe");
        return false;
    }
    set_nonblocking(_signal_pipe[0]);
    set_nonblocking(_signal_pipe[1]);

    _listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenfd < 0) {
        perror("socket");
        return false;
    }

    int optval = 1;
    if (setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        return false;
    }

    struct sockaddr_in servaddr = {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(_port.c_str()));

    if (bind(_listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        return false;
    }

    if (listen(_listenfd, SOMAXCONN) < 0) {
        perror("listen");
        return false;
    }

    _epollfd = epoll_create(MAX_CLIENTS);
    if (_epollfd < 0) {
        perror("epoll_create");
        return false;
    }

    epoll_event ev = {};
    ev.events = EPOLLIN;
    ev.data.fd = _signal_pipe[0];
    if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _signal_pipe[0], &ev) < 0) {
        perror("epoll_ctl: signal_pipe");
        return false;
    }

    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = _listenfd;
    if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listenfd, &ev) < 0) {
        perror("epoll_ctl: listenfd");
        return false;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    return true;
}

int EchoServer::run()
{
    if (!init()) {
        return 1;
    }

    _is_running = true;
    std::cout << "Server running on port " << _port << std::endl;

    epoll_event events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

    while (_is_running) {
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

            if (fd == _signal_pipe[0]) {
                char buf[16];
                while (read(_signal_pipe[0], buf, sizeof(buf)) > 0) {}
                std::cout << "\nSignal received, shutting down..." << std::endl;
                _is_running = false;
            }
            else if (fd == _listenfd) {
                sockaddr_in cliaddr = {};
                socklen_t clilen = sizeof(cliaddr);
                int connfd = accept(_listenfd, (struct sockaddr*)&cliaddr, &clilen);

                if (connfd < 0) {
                    perror("accept");
                    continue;
                }

                if (_clients.size() >= MAX_CLIENTS) {
                    std::cout << "Max clients reached" << std::endl;
                    close(connfd);
                    continue;
                }

                std::cout << "New connection: fd=" << connfd
                          << " (total: " << _clients.size() + 1 << ")"
                          << std::endl;

                add_client(connfd);

                epoll_event ev = {};
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, connfd, &ev) < 0) {
                    perror("epoll_ctl: client");
                    remove_client(connfd);
                    close(connfd);
                }
            }
            else {
                ssize_t n = read(fd, buffer, BUFFER_SIZE);

                if (n <= 0) {
                    if (n < 0) perror("read");
                    std::cout << "Connection closed: fd=" << fd
                              << " (remaining: " << _clients.size() - 1 << ")"
                              << std::endl;
                    epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    remove_client(fd);
                } else {
                    write(fd, buffer, n);
                }
            }
        }
    }

    return 0;
}