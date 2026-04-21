#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoll{
private:
    static const int MaxEvents = 100;
    int epollfd = -1;
    struct epoll_event events_[MaxEvents];
public:
    Epoll();
    ~Epoll();

    void addfd(int fd, uint32_t op);
    std::vector<epoll_event> loop(int out = -1);

};