#include "lib_echo.hpp"

bool configure_address(const std::string& hostname,
                              const std::string& port,
                              addrinfo** peer_address) {
    addrinfo hints = addrinfo();
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, peer_address) != 0) {
        std::cerr << "getaddrinfo() failed. (" << GETSOCKETERRNO() << ")\n";
        return false;
    }
    return true;
}

void print_address(const addrinfo* peer_address) {
    char address_buffer[ADDRESS_BUFFER_SIZE] = {0};
    char service_buffer[ADDRESS_BUFFER_SIZE] = {0};

    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);

    std::cout << "Remote address is: " << address_buffer
              << " " << service_buffer << "\n";
}

SOCKET create_and_connect(const addrinfo* peer_address) {
    SOCKET socket_peer = socket(peer_address->ai_family,
                                peer_address->ai_socktype,
                                peer_address->ai_protocol);

    if (!ISVALIDSOCKET(socket_peer)) {
        std::cerr << "socket() failed. (" << GETSOCKETERRNO() << ")\n";
        return socket_peer;
    }

    if (connect(socket_peer, peer_address->ai_addr,
                peer_address->ai_addrlen) != 0) {
        std::cerr << "connect() failed. (" << GETSOCKETERRNO() << ")\n";
        CLOSESOCKET(socket_peer);
        return -1;
                }

    return socket_peer;
}

bool handle_socket_read(SOCKET socket_peer) {
    char buffer[BUFFER_SIZE] = {0};

    int bytes_received = recv(socket_peer, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 1) {
        std::cout << "Connection closed by peer.\n";
        return false;
    }

    std::cout << "Received (" << bytes_received << " bytes): ";
    std::cout.write(buffer, bytes_received);
    std::cout.flush();

    return true;
}


