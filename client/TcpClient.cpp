#include "TcpClient.hpp"

TcpClient::TcpClient(const std::string& hostname, const std::string& port)
        : _hostname(hostname), _port(port), _peer_address(), _socket_peer()
{
    std::cout << "Configuring remote address...\n";
    if (!configure_address(hostname, port, &_peer_address)) {
        throw std::runtime_error("Failed to configure address.");
    }
    print_address(_peer_address);
}

TcpClient::~TcpClient()
{
    std::cout << "Closing socket...\n";
    CLOSESOCKET(_socket_peer);
    std::cout << "Finished.\n";
}

void TcpClient::init()
{

    std::cout << "Creating socket...\n";
    _socket_peer = socket_from_address(_peer_address);
    freeaddrinfo(_peer_address);

    std::cout << "Connecting...\n";
    if (connect(_socket_peer, _peer_address->ai_addr,
                _peer_address->ai_addrlen) != 0) {
        std::cerr << "connect() failed. (" << GETSOCKETERRNO() << ")\n";
        CLOSESOCKET(_socket_peer);
        throw std::runtime_error("Failed to connect to socket.");
                }
    if (!ISVALIDSOCKET(_socket_peer)) {
        throw std::runtime_error("Failed to connect to socket.");
    }
    std::cout << "Connected.\n";
}

bool TcpClient::stdin_to_peer() const
{
    std::string line;

    if (!std::getline(std::cin, line)) {
        return false;
    }
    line += "\n";

    std::cout << "Sending: " << line << std::flush;

    int bytes_sent = send(_socket_peer, line.c_str(), line.length(), 0);
    std::cout << "Sent " << bytes_sent << " bytes.\n";

    return true;
}

void TcpClient::run_loop() const
{
    fd_set reads;
    timeval timeout = {};

    while(true) {
        FD_ZERO(&reads);
        FD_SET(_socket_peer, &reads);
        FD_SET(0, &reads);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(_socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
            throw std::runtime_error("select() failed." + std::string(strerror(GETSOCKETERRNO())));
        }

        if (FD_ISSET(_socket_peer, &reads)) {
            if (!handle_socket_read(_socket_peer)) {
                break;
            }
        }

        if (FD_ISSET(0, &reads)) {
            if (!stdin_to_peer()) {
                break;
            }
        }
    }
}


