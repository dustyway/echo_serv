# Reactor Pattern Architecture

## Overview

This implements the **Reactor pattern** - a design pattern for handling concurrent I/O using event demultiplexing.

```
┌─────────────────────────────────────────────────────────┐
│                        Server                           │
│  - Owns the Reactor and Acceptor                        │
│  - Orchestrates startup/shutdown                        │
└─────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────┐
│                        Reactor                          │
│  - Central event loop (epoll_wait)                      │
│  - Maps fd → EventHandler                               │
│  - Dispatches events to handlers                        │
└─────────────────────────────────────────────────────────┘
                            │
            ┌───────────────┴───────────────┐
            ▼                               ▼
┌───────────────────────┐       ┌───────────────────────┐
│       Acceptor        │       │   ConnectionHandler   │
│  - Listens on port    │       │  - Per-client handler │
│  - Accepts new conns  │       │  - Echo logic         │
│  - Creates handlers   │       │  - Read/write buffers │
└───────────────────────┘       └───────────────────────┘
```

## Component Details

### Reactor (`Reactor.cpp`)
- **Core**: Uses Linux `epoll` to monitor multiple file descriptors
- **`run()`**: Blocks on `epoll_wait()`, then dispatches events:
  - `EPOLLIN` → `handler->handle_read()`
  - `EPOLLOUT` → `handler->handle_write()`
  - `EPOLLERR/EPOLLHUP` → `handler->handle_close()`
- **`_handlers` map**: Tracks fd → EventHandler associations

### Acceptor (`Acceptor.cpp`)
- **`open()`**: Creates listening socket, binds to port, registers with Reactor for `EPOLLIN`
- **`handle_read()`**: Called when new connections arrive:
  1. `accept()` the connection
  2. Create a new `ConnectionHandler` for that client
  3. Register the handler with the Reactor

### ConnectionHandler (`ConnectionHandler.cpp`)
- One instance per connected client
- **`handle_read()`**: Reads data into `_write_buffer` (for echo), enables `EPOLLOUT`
- **`handle_write()`**: Writes buffered data back to client, disables `EPOLLOUT` when done
- **`handle_close()`**: Cleans up and `delete this`

## Flow Example

1. **Startup**: `Server::run()` → `Reactor::init()` → `Acceptor::open()` → `Reactor::run()`
2. **Client connects**: Reactor wakes → `Acceptor::handle_read()` → creates `ConnectionHandler`
3. **Client sends data**: Reactor wakes → `ConnectionHandler::handle_read()` → buffers data
4. **Echo back**: Reactor wakes (EPOLLOUT) → `ConnectionHandler::handle_write()` → sends data
5. **Client disconnects**: `handle_close()` → removes handler → deletes itself
