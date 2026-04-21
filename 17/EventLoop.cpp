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
    while(true){
        
        std::vector<Channel*> channels = ep_->loop();
        
        for(auto& ch : channels){
            ch->handleEvent();
            
        }
    }
}


void EventLoop::updateChannel(Channel *ch)
{
    ep_->updateChannel(ch);
}