#pragma once
#include <functional>
#include "Epoll.h"
#include <memory>
#include <unistd.h>
#include <syscall.h>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include "Channel.h"

class EventLoop {
private:
    Epoll ep_;
    pid_t threadid_;                                    //循环所处的线程ID
    std::queue<std::function<void()>> taskqueue_;       //独属于IO线程的任务队列 
    std::mutex mutex_;                                  //用于保护队列被工作线程互斥访问的锁
    int wakeupfd_;                                      //用于唤醒IO线程处理等待队列的fd
    Channel wakeupChannel_;                             //唤醒队列的Channel

    std::function<void(EventLoop*)> epollTimeoutCallback_;
public:
    EventLoop();
    ~EventLoop();

    void run();
    void updateChannel(Channel *ch);
    void removeChannel(Channel *ch);

    bool isinLoopThread();
    void queueInLoop(std::function<void()> fn);

    void wakeUp();
    void handleWakeUp();

    void setepollTimeoutCallback(std::function<void(EventLoop*)> fn);
};