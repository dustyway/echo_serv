#include "Server.hpp"

int main(int argc, char* argv[])
{
    Server server(argc > 1 ? argv[1] : "8080");
    return server.run();
}