#pragma once
#include <cstdint>
class Epoll;

class Channel{
private:
    int fd_ = -1;
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    bool inepoll = false;
    Epoll *ep_ = nullptr; 
public:
    Channel(int fd, Epoll *ep);
    ~Channel();
    
    int fd();
    bool inpoll();
    void setinpoll();
    void setET();
    void setrevents(uint32_t ev);
    void enableReading();

    uint32_t events();
    uint32_t revents();
};