#ifndef ECHO_SERV_CLIENT_HPP
#define ECHO_SERV_CLIENT_HPP

#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <cstring>
#include "lib_echo.hpp"

class TcpClient
{
public:
    TcpClient(const std::string &hostname, const std::string &port);
    ~TcpClient();
    void init();
    void run_loop() const;

private:
    TcpClient(const TcpClient&);
    TcpClient& operator=(const TcpClient&);
    bool stdin_to_peer() const;
    std::string _hostname;
    std::string _port;
    addrinfo* _peer_address;
    SOCKET _socket_peer;
};

#endif