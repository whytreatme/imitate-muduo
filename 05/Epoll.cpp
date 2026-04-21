#include "Epoll.h"
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>


Epoll::Epoll(){
    if((epollfd = epoll_create(1)) == -1){
        perror("epoll_create() failed.");
        exit(-1);
    }
}

void Epoll::addfd(int fd, uint32_t op){
    struct epoll_event ev;
    ev.events = op;
    ev.data.fd = fd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1){
        perror("epoll_ctl: listen_sock");
        exit(-1);
    }
}

std::vector<epoll_event> Epoll::loop(int timeout){
    std::vector<epoll_event> evs;
   
    int nfds = epoll_wait(epollfd, events_,  MaxEvents, timeout);
    if(nfds < 0){
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }
    else if(nfds == 0){
        printf("epoll_wait() timeout.\n");
        return evs;
    }
    for(int i = 0; i < nfds; i++){
        evs.push_back(events_[i]);
    }
    return evs;
}

Epoll::~Epoll(){
    ::close(this->epollfd);
}