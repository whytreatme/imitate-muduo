#pragma once
#include <functional>
#include "Epoll.h"

class EventLoop{
private:
    Epoll *ep_;

    std::function<void(EventLoop*)> epollTimeoutCallback_;
public:
    EventLoop();
    ~EventLoop();
    void run();
    void updateChannel(Channel *ch);
    void removeChannel(Channel *ch);

    void setepollTimeoutCallback(std::function<void(EventLoop*)> fn);
};