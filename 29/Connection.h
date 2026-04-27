#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <iostream>
#include <memory>
#include "Buffer.h"

class Connection;
using spConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection>{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientChannel_;
    Buffer inputbuffer_;        //增加接受缓冲区
    Buffer outputbuffer_;        //发送接受缓冲区
    //回调函数区
    std::function<void(spConnection)> closeCallback_;
    std::function<void(spConnection)> errorCallback_;
    std::function<void(spConnection, std::string&)> onMessageCallback_;
    std::function<void(spConnection)> sendCompleteCallback_;
    
public:
    Connection(EventLoop *loop, Socket *);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;
    void errorCallback();
    void closeCallback();
    void onMessage();
    void send(const char *data, size_t size);
    void writeCallback();

    void setErrorCallback(std::function<void(spConnection)> fn);
    void setCloseCallback(std::function<void(spConnection)> fn);
    void setonMessageCallback(std::function<void(spConnection, std::string&)> fn);
    void setsendCompleteCallback(std::function<void(spConnection)> fn);

    void ConnectEstablished();
};