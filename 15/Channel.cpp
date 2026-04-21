#include "Channel.h"
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include "EventLoop.h"
#include "InetAddress.h"
#include "cstring"
#include "Socket.h"




Channel::Channel(int fd, EventLoop *loop) : fd_(fd), loop_(loop)
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
    // this->ep_->updateChannel(this);
    loop_->updateChannel(this);
}

void Channel::handleEvent(){
    if(revents_ & EPOLLRDHUP){
        std::cout << "1client(eventfd= " << fd_ <<") disconnected.\n";
        close(fd_);
    }
    else if(revents_ & (EPOLLIN | EPOLLPRI)){
        readcallback_();
    } 
}

void Channel::setReadcallback(std::function<void()> fn){
    readcallback_ = fn;
}



void Channel::onMessage(){
    char buffer[1024];
    while(true){
        memset(buffer, 0, sizeof(buffer));
        ssize_t nread = recv(fd_, buffer, sizeof(buffer) ,0);
        if((nread < 0) && ((errno == EAGAIN) ||(errno == EWOULDBLOCK))) break;
        else if(nread == 0){
            std::cout << "2client(eventfd= " << fd_ <<") disconnected.\n";
            close(fd_);
            break;
        }
        else if((nread < 0) && (errno == EINTR)) continue;
        else{
            printf("recv(eventfd=%d):%s\n",fd_,buffer);
            send(fd_,buffer,strlen(buffer),0);
        }
    }
}