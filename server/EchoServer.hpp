#ifndef ECHOSERVEER_HPP
#define ECHOSERVEER_HPP

#include "lib_echo.hpp"

class EchoServer
{
public:
    EchoServer(const std::string &port);
    ~EchoServer();
    void run_loop();

private:
    EchoServer(const EchoServer&);
    EchoServer& operator=(const EchoServer&);
    // bool stdin_to_peer() const;
    // std::string _hostname;
    std::string _port;
    // addrinfo* _peer_address;
    // SOCKET _socket_peer;
};

#endif