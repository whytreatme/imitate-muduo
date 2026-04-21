#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"

class EchoServer{
private:
    TcpServer tcpserver_;
public:
    EchoServer(const std::string &ip, const uint16_t port);
    ~EchoServer();

    void Start();    //启动服务

    //回调作为抓手操作底层类
    void HandleNewConnection(Connection *);
    void HandleMessage(Connection *, std::string&);
    void HandleClose(Connection*);
    void HandleError(Connection*);
    void HandleSendComplete(Connection*);
    //void HandleTimeout(EventLoop*);
};