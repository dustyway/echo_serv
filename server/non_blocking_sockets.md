# Non-blocking Sockets in the Reactor Pattern

## Why is the fd in Acceptor set as non-blocking?

The listening socket is set non-blocking to handle a race condition in `handle_read()`.

When `epoll_wait` returns with `EPOLLIN` on the listening socket, it means at least one connection is pending. However, by the time you call `accept()`, that connection might have:
- Been reset by the client (TCP RST)
- Timed out
- Been closed

If the socket were **blocking** and no connections remained, `accept()` would block the entire event loop, freezing all other clients.

With **non-blocking**, `accept()` returns immediately with `EAGAIN`/`EWOULDBLOCK` if nothing is pending, allowing the loop in `handle_read()` to break cleanly:

```cpp
// Acceptor.cpp:62-76
while (true) {
    int connfd = accept(_fd, ...);
    if (connfd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break;  // No more pending connections - exit gracefully
        }
        ...
    }
    ...
}
```

This also enables accepting **multiple connections** in one `handle_read()` call - the loop keeps accepting until `EAGAIN`, which is more efficient than waking up once per connection.

---

## Could it be done with blocking sockets?

Yes, but with limitations:

**With blocking sockets:**
- You'd remove the `while(true)` loop and call `accept()` only once per `EPOLLIN` event
- It would mostly work since `epoll` tells you a connection is ready before you call `accept()`

**Problems:**

1. **The race condition** - Between `epoll_wait` returning and `accept()` being called, the client could disconnect. With blocking, your entire server hangs. Rare, but possible.

2. **One accept per wake** - You'd need one `epoll_wait` wake per connection instead of draining all pending connections at once. Less efficient under high connection rates.

3. **ConnectionHandler breaks** - The bigger issue is `ConnectionHandler`. With blocking `read()`:
   - If a client sends partial data and stops, `read()` blocks waiting for more
   - All other clients freeze

4. **Write blocking** - If a client's receive buffer is full, `write()` blocks until it drains. Again, all clients freeze.

**Bottom line:** Blocking sockets work for single-client servers or thread-per-client models. The reactor pattern's entire purpose is multiplexing many clients on one thread, which fundamentally requires non-blocking I/O to avoid one slow client blocking everyone else.
