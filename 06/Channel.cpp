#include "Channel.h"
#include <sys/epoll.h>
#include "Epoll.h"

/*
    class Channel{
private:
    int fd_;
    uint32_t events_;
    uint32_t revents_;
    bool inepoll = false;
    Epoll *ep_; 
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
*/

Channel::Channel(int fd, Epoll *ep) : fd_(fd), ep_(ep)
{

}  

int Channel::fd(){
    return this->fd_;
}

bool Channel::inpoll(){
    return this->inepoll;
}

void Channel::setinpoll(){
    this->inepoll = true;
}

void Channel::setET(){
    this->events_ |= EPOLLET;
}

void Channel::setrevents(uint32_t ev) {
    this->revents_ = ev;
}

uint32_t Channel::events(){
    return this->events_;
}

uint32_t Channel::revents(){
    return this->revents_;
}

Channel::~Channel(){}

void Channel::enableReading(){
    this->events_ |= EPOLLIN;
    this->ep_->updateChannel(this);
}