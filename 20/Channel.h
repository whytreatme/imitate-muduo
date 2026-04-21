#pragma once
#include <cstdint>
#include <functional>
#include "Socket.h"
class EventLoop;

class Channel{
private:
    int fd_ = -1;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    bool inepoll = false;
    EventLoop *loop_ = nullptr; 
    std::function<void()> readCallback_;
    std::function<void()> errorCallback_;
    std::function<void()> closeCallback_;
        
public:
    Channel(int fd, EventLoop *ep);
    ~Channel();
    
    int fd();
    bool inpoll();
    bool islisten();
    void setinpoll();
    void setET();
    void setrevents(uint32_t ev);
    void enableReading();
    void handleEvent();
    //void newConnection(Socket *servsock);
   // void onMessage();
    void setReadcallback(std::function<void()> fn);
    void setClosecallback(std::function<void()> fn);
    void setErrorcallback(std::function<void()> fn);

    uint32_t events();
    uint32_t revents();
};