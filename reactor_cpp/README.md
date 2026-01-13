# Reactor Pattern - C++98 OOP Implementation

This directory contains an Object-Oriented C++98 implementation of the Reactor pattern for network event handling.

## Architecture Overview

### Class Hierarchy

```
EventHandler (Abstract Base Class)
├── DiagnosticsClient
└── DiagnosticsServer

Reactor (Abstract Base Class)
├── PollReactor
└── SelectReactor

ServerEventNotifier (Interface)
└── DiagnosticsServer

TcpServer (Utility Class)
```

### Key Classes

#### EventHandler
Abstract base class defining the interface for event handlers:
- `virtual Handle getHandle() const = 0` - Returns the file descriptor to monitor
- `virtual void handleEvent() = 0` - Called when an event occurs on the handle

#### Reactor
Abstract base class for the reactor pattern:
- `virtual void registerHandler(EventHandler*)` - Register an event handler
- `virtual void unregisterHandler(EventHandler*)` - Unregister an event handler
- `virtual void handleEvents()` - Event demultiplexing loop

#### PollReactor
Concrete implementation using `poll()` system call:
- Uses `struct pollfd` for event multiplexing
- Supports up to 32 concurrent handles
- More scalable than select for large numbers of file descriptors

#### SelectReactor
Concrete implementation using `select()` system call:
- Uses `fd_set` bit masks for event multiplexing
- Supports up to 32 concurrent handles
- More portable across older Unix systems
- Limited to FD_SETSIZE file descriptors

#### DiagnosticsServer
Server that accepts client connections:
- Inherits from EventHandler for connection events
- Implements ServerEventNotifier for client callbacks
- Uses TcpServer for socket management
- Manages up to 10 concurrent client connections
- Automatically registers/unregisters handlers with Reactor

#### DiagnosticsClient
Client connection handler:
- Inherits from EventHandler for data events
- Receives diagnostic messages from clients
- Notifies server when connection closes
- Uses RAII for resource management

#### TcpServer
Encapsulates TCP server socket creation:
- Constructor creates and binds listening socket
- Destructor automatically closes socket
- Uses RAII principles for resource management

## OOP Improvements Over C Version

1. **Polymorphism**: Virtual functions replace function pointers
2. **Encapsulation**: Data and methods grouped in classes
3. **RAII**: Automatic resource management via constructors/destructors
4. **Type Safety**: Strong typing with C++ type system
5. **Memory Management**: Uses `new`/`delete` instead of `malloc`/`free`
6. **Cleaner Interfaces**: Abstract base classes define clear contracts
7. **No Manual Vtables**: C++ handles virtual dispatch automatically

## Building

Two executables are built with different reactor implementations:

```bash
cmake ..
make

# Poll-based reactor
./tcpserv_poll

# Select-based reactor
./tcpserv_select
```

## Implementation Details

### Event Flow

1. Main creates a Reactor instance (Poll or Select)
2. DiagnosticsServer is created, passing the Reactor
3. Server registers itself with the Reactor
4. Event loop calls `reactor.handleEvents()`
5. Reactor monitors file descriptors
6. When connection arrives, server's `handleEvent()` is called
7. Server creates DiagnosticsClient and registers it
8. When client data arrives, client's `handleEvent()` is called
9. When connection closes, client notifies server via callback
10. Server unregisters and deletes the client

### Key Design Patterns

- **Reactor Pattern**: Demultiplex and dispatch event handlers
- **Strategy Pattern**: Interchangeable reactor implementations
- **Observer Pattern**: Handlers observe file descriptor events
- **RAII**: Resource Acquisition Is Initialization
- **Interface Segregation**: Multiple small interfaces

## C++98 Compliance

This code strictly follows C++98 standard:
- No C++11 features (auto, nullptr, range-for, lambdas, etc.)
- Uses NULL instead of nullptr
- Uses typedef instead of using
- Loop variables declared before loops
- No uniform initialization
- Manual memory management (no smart pointers)

## Performance Considerations

### Poll vs Select

**PollReactor (poll())**:
- O(n) scalability with number of file descriptors
- No hard limit on number of descriptors
- Slightly more efficient for large numbers of FDs

**SelectReactor (select())**:
- O(n) but with FD_SETSIZE limit (typically 1024)
- More portable to older systems
- Simpler bit mask interface
- Must track maximum FD manually

## Testing

Connect to the server using telnet or netcat:
```bash
# Terminal 1
./tcpserv_poll

# Terminal 2
telnet localhost 9877
```

Send messages and observe diagnostic output on the server.
