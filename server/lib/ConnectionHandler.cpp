#include "ConnectionHandler.hpp"

ConnectionHandler::ConnectionHandler(int fd, Reactor& reactor)
    : _reactor(reactor)
{
    _fd = fd;
}

ConnectionHandler::~ConnectionHandler()
{
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }
}

void ConnectionHandler::handle_read()
{
    char buffer[BUFFER_SIZE];
    ssize_t n = read(_fd, buffer, sizeof(buffer));

    if (n > 0) {
        // Echo: append to write buffer
        _write_buffer.append(buffer, n);

        // Enable write notifications if we have data to send
        if (!_write_buffer.empty()) {
            _reactor.modify_handler(this, EPOLLIN | EPOLLOUT);
        }
    } else if (n == 0) {
        // EOF - peer closed connection gracefully
        handle_close();
    } else {
        // n < 0: error - close connection
        handle_close();
    }
}

void ConnectionHandler::handle_write()
{
    if (_write_buffer.empty()) {
        _reactor.modify_handler(this, EPOLLIN);
        return;
    }

    ssize_t n = write(_fd, _write_buffer.data(), _write_buffer.size());

    if (n > 0) {
        _write_buffer.erase(0, n);

        // Disable write notifications if buffer is empty
        if (_write_buffer.empty()) {
            _reactor.modify_handler(this, EPOLLIN);
        }
    } else {
        // n <= 0: error - close connection
        handle_close();
    }
}

void ConnectionHandler::handle_close()
{
    std::cout << "Connection closed: fd=" << _fd << std::endl;
    _reactor.remove_handler(this);
    delete this;
}
