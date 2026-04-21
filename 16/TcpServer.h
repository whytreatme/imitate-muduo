#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include <string>
#include "Acceptor.h"
#include "Connection.h"
#include <map>

class TcpServer{
private:
    EventLoop loop_;
    Acceptor *acceptor_;
    std::map<int, Connection*>conns_;
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();
    void start();

    void newConnection(Socket *);
};