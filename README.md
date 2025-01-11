# To be a better Backend Developer

### Why you need to know socket programming
The reason backend developers need to know socket programming is to understand the basic principles of network communication and use them to develop a variety of server and network applications. Socket programming is a key skill for network-based backend systems, and knowing it allows you to design more efficient and reliable systems. **HTTP will eventually return to being TCP/IP-based.**

### What i learned
- [x] Basic TCP Socket Programming
    - [x] Check `3-Way handshake` in `Wireshark`
    - [x] TCP Echo Service Full Flow
    - [x] Creating a TCP Echo Server
        - [x] Waiting for Server socket
        - [x] How to receiving and communicating with Client socket
        - [x] Creating a TCP Echo Client
        - [x] Test Echo Client/Server and check out `Wireshark`
- [x] Socket I/O buffers
    - [x] What is `packet` and `segement`
    - [x] How to work `Nagle algorithms`
    - [x] The `TCP_NODELAY` option
    - [x] Why the server sockets need `SO_REUSEADDR` option
    - [x] Multi-thread Echo Server
- [x] TCP Chat Server
    - [x] Multi-thread-based chat server structure
    - [x] Multi-thread-based chat server-client
    - [x] Forcing a server to crash
    - [x] MFC Chat Server and Client (just viewed)
    - [x] What is I/O multiplexing
    - [x] With I/O multiplexing and event
- [x] Send/receive files and Protocol design
    - [x] Create a file transfer server and client
    - [x] What is `Zero-window`
    - [x] Create a file sending server with a protocol
    - [x] What is `Lookup Table`
- [x] Improve server performance
    - [x] Event-driven asynchronous file I/O
    - [x] Callback-driven asynchronous file I/O
    - [x] Why occur a blue screen
    - [x] IOCP Model
- [x] UDP and Broadcasting
    - [x] Send/receive UDP broadcasts
- [x] DNS query methods
- [x] Simple HTTP, FTP Client
