#pragma once
#include "InetAddress.h"  

int create_non_block();

class Socket{
private:
    const int fd_;
public:
    Socket(int fd);
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    ~Socket();
    int fd() const;
    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void settcpnodelay(bool on);

    void bind(InetAddress&) const;
    void listen(int size = 128);
    int accept(InetAddress&);
};