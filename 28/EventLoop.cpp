#include "EventLoop.h"
#include "Channel.h"



EventLoop::EventLoop()  : ep_(new Epoll)
{

}

EventLoop::~EventLoop()
{
    delete ep_;
}


void EventLoop::run()
{
    //printf("EventLoop::run() thread is %ld.\n", syscall(SYS_gettid));
    while(true){
        
        std::vector<Channel*> channels = ep_->loop(10*1000);
        
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
    ep_->updateChannel(ch);
}

void EventLoop::setepollTimeoutCallback(std::function<void(EventLoop*)> fn)
{
    epollTimeoutCallback_ = fn;
}