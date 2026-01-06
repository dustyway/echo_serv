
#include "client.hpp"

static void run_client_loop(SOCKET socket_peer) {
    fd_set reads;
    struct timeval timeout;
    bool running = true;

    while(running) {
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);
        FD_SET(0, &reads);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
            std::cerr << "select() failed. (" << GETSOCKETERRNO() << ")\n";
            break;
        }

        if (FD_ISSET(socket_peer, &reads)) {
            if (!handle_socket_read(socket_peer)) {
                running = false;
            }
        }

        if (running && FD_ISSET(0, &reads)) {
            if (!handle_stdin_read(socket_peer)) {
                running = false;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    struct addrinfo* peer_address = 0;
    int exit_code = 0;

    if (argc < 3) {
        std::cerr << "usage: tcp_client hostname port\n";
        return 1;
    }

    std::string hostname(argv[1]);
    std::string port(argv[2]);

    std::cout << "Configuring remote address...\n";
    if (!configure_address(hostname, port, &peer_address)) {
        return 1;
    }

    print_address(peer_address);

    std::cout << "Creating socket...\n";
    std::cout << "Connecting...\n";

    SOCKET socket_peer = create_and_connect(peer_address);
    freeaddrinfo(peer_address);

    if (!ISVALIDSOCKET(socket_peer)) {
        return 1;
    }

    std::cout << "Connected.\n";
    std::cout << "To send data, enter text followed by enter.\n";

    run_client_loop(socket_peer);

    std::cout << "Closing socket...\n";
    CLOSESOCKET(socket_peer);

    std::cout << "Finished.\n";
    return exit_code;
}