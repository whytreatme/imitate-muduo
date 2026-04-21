#include "Epoll.h"
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include "Channel.h"


Epoll::Epoll(){
    if((epollfd = epoll_create(1)) == -1){
        perror("epoll_create() failed.");
        exit(-1);
    }
}

void Epoll::updateChannel(Channel *ch){
    struct epoll_event ev;
    ev.events = ch->events();
    ev.data.ptr = ch;
    if(ch->inpoll()){
        if(epoll_ctl(this->epollfd, EPOLL_CTL_MOD, ch->fd(), &ev) == -1){
            perror("epoll_ctl failed:");
            exit(-1);
        }
    }
    else{
        if(epoll_ctl(this->epollfd, EPOLL_CTL_ADD, ch->fd(), &ev) == -1){
            perror("epoll_ctl failed:");
            exit(-1);
        }
        ch->setinpoll();
    }
}


std::vector<Channel*> Epoll::loop(int timeout){
    std::vector<Channel*> channels;
   
    int nfds = epoll_wait(epollfd, events_,  MaxEvents, timeout);
    if(nfds < 0){
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }
    else if(nfds == 0){
        printf("epoll_wait() timeout.\n");
        return channels;
    }
    for(int i = 0; i < nfds; i++){
        Channel *ch = (Channel*)events_[i].data.ptr;
        ch->setrevents(events_[i].events);
        channels.push_back(ch);
    }
    return channels;
}

Epoll::~Epoll(){
    ::close(this->epollfd);
}