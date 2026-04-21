#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <string>

class Acceptor{
private:
    EventLoop *loop_;
    Socket *servsock_;
    Channel *acceptChannel_;
    std::function<void(Socket*)> newConnectioncb_;
public:
    Acceptor(EventLoop *loop, const std::string &ip, uint16_t port);
    ~Acceptor();

    void newConnection();
    void setnewConnection( std::function<void(Socket*)> fn);
};