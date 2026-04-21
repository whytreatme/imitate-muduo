#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include <string>

class TcpServer{
private:
    EventLoop loop_;
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();
    void start();
};