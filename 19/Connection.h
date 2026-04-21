#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <iostream>
#include "Buffer.h"


class Connection{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientChannel_;
    Buffer inputbuffer_;        //增加接受缓冲区
    Buffer outputbuffer_;        //发送接受缓冲区
    std::function<void(Connection*)> closeCallback_;
    std::function<void(Connection*)> errorCallback_;
    
public:
    Connection(EventLoop *loop, Socket *);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;
    void errorCallback();
    void closeCallback();
    void onMessage();              //处理对端发来的消息
    void setErrorCallback(std::function<void(Connection*)> fn);
    void setCloseCallback(std::function<void(Connection*)> fn);
    
};