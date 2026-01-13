#ifndef TCP_SERVER_H
#define TCP_SERVER_H

/**
 * TcpServer class encapsulates TCP server socket creation and management.
 * Uses RAII principles for resource management.
 */
class TcpServer
{
public:
    /**
     * Create a TCP server socket listening on the specified port.
     * @param tcpPort Port number to listen on
     */
    explicit TcpServer(unsigned int tcpPort);

    /**
     * Destructor closes the server socket.
     */
    ~TcpServer();

    /**
     * Get the server socket file descriptor.
     * @return The server socket handle
     */
    int getSocket() const;

private:
    int serverSocket_;

    // Prevent copying
    TcpServer(const TcpServer&);
    TcpServer& operator=(const TcpServer&);
};

#endif
