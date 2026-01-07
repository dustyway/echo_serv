#include "EchoClient.hpp"

EchoClient::EchoClient(const std::string& hostname, const std::string& port)
        : _running(false), _hostname(hostname),
          _port(port), _peer_address(), _socket_peer()
{
    std::cout << "Configuring remote address...\n";
    if (!configure_address(hostname, port, &_peer_address)) {
        throw std::runtime_error("Failed to configure address.");
    }
    print_address(_peer_address);

    std::cout << "Creating socket...\n";
    std::cout << "Connecting...\n";

    _socket_peer = create_and_connect(_peer_address);
    freeaddrinfo(_peer_address);

    if (!ISVALIDSOCKET(_socket_peer)) {
        throw std::runtime_error("Failed to connect to socket.");
    }

    std::cout << "Connected.\n";
}

EchoClient::~EchoClient()
{
    std::cout << "Closing socket...\n";
    CLOSESOCKET(_socket_peer);
    std::cout << "Finished.\n";
}

void EchoClient::run_loop()
{
    fd_set reads;
    timeval timeout = {};
    _running = true;

    while(_running) {
        FD_ZERO(&reads);
        FD_SET(_socket_peer, &reads);
        FD_SET(0, &reads);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(_socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
            std::cerr << "select() failed. (" << GETSOCKETERRNO() << ")\n";
            break;
        }

        if (FD_ISSET(_socket_peer, &reads)) {
            if (!handle_socket_read(_socket_peer)) {
                _running = false;
            }
        }

        if (_running && FD_ISSET(0, &reads)) {
            if (!handle_stdin_read(_socket_peer)) {
                _running = false;
            }
        }
    }
}

