#include "TcpClient.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3 ) {
        std::cerr << "usage: tcp_client hostname port\n";
        return 1;
    }

    TcpClient client(argv[1], argv[2]);;

    std::cout << "To send data, enter text followed by enter.\n";
    client.run_loop();

    return 0;
}