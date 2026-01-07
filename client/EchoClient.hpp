#ifndef ECHO_SERV_CLIENT_HPP
#define ECHO_SERV_CLIENT_HPP

#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include "lib_echo.hpp"

class EchoClient
{
public:
    EchoClient(const std::string &hostname, const std::string &port);
    ~EchoClient();
    void run_loop();

private:
    EchoClient(const EchoClient&);
    EchoClient& operator=(const EchoClient&);
    bool _running;
    std::string _hostname;
    std::string _port;
    addrinfo* _peer_address;
    SOCKET _socket_peer;
};

#endif