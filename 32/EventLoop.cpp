#include "EventLoop.h"
#include "Channel.h"



EventLoop::EventLoop() : 
                        wakeupfd_(eventfd(0, EFD_NONBLOCK)), 
                        wakeupChannel_(wakeupfd_, *this),
                        threadid_(-1)
{
    wakeupChannel_.setReadcallback(std::bind(&EventLoop::handleWakeUp, this));
    updateChannel(&wakeupChannel_);
    wakeupChannel_.enableReading();
}

EventLoop::~EventLoop()
{
    removeChannel(&wakeupChannel_);
}


void EventLoop::run()
{
    //printf("EventLoop::run() thread is %ld.\n", syscall(SYS_gettid));
    threadid_ = syscall(SYS_gettid);
    while(true){
        
        std::vector<Channel*> channels = ep_.loop(10*1000);
        
        if(channels.size() == 0) {
            epollTimeoutCallback_(this);
            //break;
        }
        else{
            for(auto& ch : channels){
                ch->handleEvent();  
            }
        }
    }
}


void EventLoop::updateChannel(Channel *ch)
{
    ep_.updateChannel(ch);
}

void EventLoop::removeChannel(Channel *ch)
{
    ep_.removeChannel(ch);
}

void EventLoop::setepollTimeoutCallback(std::function<void(EventLoop*)> fn)
{
    epollTimeoutCallback_ = fn;
}

//判断当前是否处于IO线程
bool EventLoop::isinLoopThread()
{
    return  threadid_ == syscall(SYS_gettid);
}

//将任务塞进队列中
void EventLoop::queueInLoop(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> lock_(mutex_);
        taskqueue_.push(fn);
        wakeUp();
    }
}

void EventLoop::wakeUp()
{
    uint64_t val = 1;
    write(wakeupfd_, &val, sizeof(val));
}

void EventLoop::handleWakeUp()
{
    printf("Now Thread(%ld) runs EventLoop::handleWakeUp .\n", syscall(SYS_gettid));
    uint64_t val;
    read(wakeupfd_, &val, sizeof(val));

    std::function<void()> task;
    {
        std::lock_guard<std::mutex> lock_(mutex_);
        while(taskqueue_.size() > 0){
            task = std::move(taskqueue_.front());
            taskqueue_.pop();
            task();
        }
    }
}