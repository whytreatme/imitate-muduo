#include "EventLoop.h"
#include "Channel.h"

/*
class EventLoop{
private:
    Epoll *ep_;
public:
    EventLoop();
    ~EventLoop();
    void run();
    Epoll* ep();
};
*/

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

Epoll* EventLoop::ep()
{
    return ep_;
}