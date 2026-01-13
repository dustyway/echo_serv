#ifndef ECHOSERVER_HPP
#define ECHOSERVER_HPP

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#define MAX_EVENTS 16
#define MAX_CLIENTS 1024
#define BUFFER_SIZE 1024

typedef int SOCKET;

class EchoServer
{
public:
    EchoServer(const std::string &port);
    ~EchoServer();
    int run();

private:
    EchoServer(const EchoServer&);
    EchoServer& operator=(const EchoServer&);

    bool init();
    void add_client(SOCKET fd);
    void remove_client(SOCKET fd);
    static void close_if_valid(SOCKET fd);

    static int _signal_pipe[2];  // Only static - for signal handler
    static void signal_handler(int sig);

    bool _is_running;  // Non-static now
    std::string _port;
    std::vector<SOCKET> _clients;
    SOCKET _listenfd;
    SOCKET _epollfd;
};

#endif