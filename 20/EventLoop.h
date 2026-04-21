#pragma once
#include "Epoll.h"

class EventLoop{
private:
    Epoll *ep_;
public:
    EventLoop();
    ~EventLoop();
    void run();
    void updateChannel(Channel *ch);
};