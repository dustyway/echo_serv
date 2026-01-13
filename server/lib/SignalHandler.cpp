#include "SignalHandler.hpp"


int SignalHandler::_pipe[2] = {-1, -1};

SignalHandler::SignalHandler(Reactor& reactor)
    : _reactor(reactor)
{
}

SignalHandler::~SignalHandler()
{
    if (_pipe[0] >= 0) {
        close(_pipe[0]);
        _pipe[0] = -1;
    }
    if (_pipe[1] >= 0) {
        close(_pipe[1]);
        _pipe[1] = -1;
    }
}

void SignalHandler::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void SignalHandler::signal_callback(int sig)
{
    (void)sig;
    int saved_errno = errno;
    write(_pipe[1], "x", 1);
    errno = saved_errno;
}

bool SignalHandler::open()
{
    if (pipe(_pipe) < 0) {
        perror("pipe");
        return false;
    }

    set_nonblocking(_pipe[0]);
    set_nonblocking(_pipe[1]);

    _fd = _pipe[0];

    if (!_reactor.register_handler(this, EPOLLIN)) {
        return false;
    }

    signal(SIGINT, signal_callback);
    signal(SIGTERM, signal_callback);

    return true;
}

void SignalHandler::handle_read()
{
    char buf[16];
    while (read(_fd, buf, sizeof(buf)) > 0) {}

    std::cout << "\nSignal received, shutting down..." << std::endl;
    _reactor.stop();
}

void SignalHandler::handle_write()
{
    // SignalHandler doesn't write
}

void SignalHandler::handle_close()
{
    _reactor.remove_handler(this);
}
