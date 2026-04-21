#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include <string>
#include "Acceptor.h"
#include "Connection.h"
#include <map>

/*
TcpServer有什么权限
TcpServer拥有Connection和Acceptor类
TcpServer拥有Acceptor创建和析构的权限
*/
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