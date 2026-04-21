#pragma once
#include "InetAddress.h"  

int create_non_block();

class Socket{
private:
    const int fd_;
    std::string ip_;
    uint16_t port_;
public:
    Socket(int fd);
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    ~Socket();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;  

    void setreuseaddr(bool on);
    void setreuseport(bool on);
    void settcpnodelay(bool on);

    void bind(InetAddress&) ;
    void listen(int size = 128);
    int accept(InetAddress&);
};