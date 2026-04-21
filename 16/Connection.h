#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"


class Connection{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientChannel_;
public:
    Connection(EventLoop *loop, Socket *);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;
};