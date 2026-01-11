#include "EchoServer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2 ) {
        std::cerr << "usage: echo_server port\n";
        return 1;
    }

    EchoServer server(argv[1]);

    for (;;) {
        HandleEvents
    }

    return 0;
}