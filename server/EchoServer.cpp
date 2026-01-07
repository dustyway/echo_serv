#include "EchoServer.hpp"

EchoServer::EchoServer(const std::string& port): _port(port)
{
    std::cout << "Configuring local address...\n";
}

EchoServer::~EchoServer()
{
}

void EchoServer::run_loop()
{
}
