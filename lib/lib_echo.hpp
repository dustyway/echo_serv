#ifndef ECHO_SERV_LIB_ECHO_HPP
#define ECHO_SERV_LIB_ECHO_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

static const int BUFFER_SIZE = 4096;
static const int ADDRESS_BUFFER_SIZE = 100;

bool parse_port(const std::string& s, unsigned int& port);
bool configure_address(const std::string& hostname,
                              const std::string& port,
                              struct addrinfo** peer_address);
void print_address(const addrinfo* peer_address);
SOCKET create_and_connect(const addrinfo* peer_address);
bool handle_socket_read(SOCKET socket_peer);

#endif