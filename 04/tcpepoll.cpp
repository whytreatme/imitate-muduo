#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstring>
#include <netinet/tcp.h>
#include "InetAddress.h"
#include "Socket.h"

using namespace std;


int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "Usage ./" << argv[0] << " ip port\n";
        cout << "./tcpepoll 192.168.38.128 5005\n\n";
        return -1;
    }

    Socket servsock(create_non_block());
   
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
   
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.bind(servaddr);
    servsock.listen();
   

    int epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    } 
#define MAX_EVENTS 10
    struct epoll_event ev, evs[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = servsock.fd();
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, servsock.fd(), &ev) == -1){
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    while(true){
        int nfds = epoll_wait(epfd, evs, MAX_EVENTS, -1);
        if(nfds < 0){
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        
        for(int i = 0; i < nfds; i++){
            if(evs[i].events & EPOLLRDHUP){
                cout << "1client(eventfd= " << evs[i].data.fd <<") disconnected.\n";
                close(evs[i].data.fd);
            }
            else if(evs[i].events & (EPOLLIN | EPOLLPRI)){
                if(evs[i].data.fd == servsock.fd()){  
                    InetAddress clientaddr;
                    Socket *clientsock = new Socket(servsock.accept(clientaddr));
                    clientsock->settcpnodelay(true);
                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());
                    struct epoll_event ev;
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = clientsock->fd();
                    if(epoll_ctl(epfd, EPOLL_CTL_ADD, clientsock->fd(), &ev) == -1){
                        perror("epoll_ctl: listen_sock");
                        exit(EXIT_FAILURE);
                    }
                }
                else{
                    char buffer[1024];
                    while(true){
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t nread = recv(evs[i].data.fd, buffer, sizeof(buffer) ,0);
                        if((nread < 0) && ((errno == EAGAIN) ||(errno == EWOULDBLOCK))) break;
                        else if(nread == 0){
                            cout << "2client(eventfd= " << evs[i].data.fd <<") disconnected.\n";
                            close(evs[i].data.fd);
                            break;
                        }
                        else if((nread < 0) && (errno == EINTR)) continue;
                        else{
                            printf("recv(eventfd=%d):%s\n",evs[i].data.fd,buffer);
                            send(evs[i].data.fd,buffer,strlen(buffer),0);
                        }
                    }
                } 
            } 
            else if(evs[i].events & EPOLLOUT){}
            else{
                cout << "3client(eventfd= " << evs[i].data.fd <<") disconnected.\n";
                close(evs[i].data.fd);
            }
            
        }
    }


    return 0;
}