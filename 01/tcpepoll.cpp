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

using namespace std;

void setnonblocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    flags = flags | O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "Usage ./" << argv[0] << " ip port\n";
        cout << "./tcpepoll 192.168.38.128 5005\n\n";
        return -1;
    }

    int listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listensock < 0)
    {
        perror("socket() failed"); return -1;
    }
    
    // 设置listenfd的属性，如果对这些属性不熟悉，百度之。
    int opt = 1; 
    setsockopt(listensock,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的。
    setsockopt(listensock,SOL_SOCKET,SO_REUSEPORT ,&opt,static_cast<socklen_t>(sizeof opt));    // 有用，但是，在Reactor中意义不大。
    
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    if(bind(listensock, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("bind() failed"); close(listensock); return -1;
    }

    if(listen(listensock, 128) < 0){
        perror("listen() failed"); close(listensock); return -1;
    }
    setnonblocking(listensock);

    int epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    } 
#define MAX_EVENTS 10
    struct epoll_event ev, evs[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listensock;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listensock, &ev) == -1){
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
            if(evs[i].data.fd == listensock){
                
                struct sockaddr_in caddr;
                socklen_t size = sizeof(caddr);
                int clientsock = accept(listensock, (struct sockaddr *)&caddr, &size);
                    setsockopt(clientsock,IPPROTO_TCP,TCP_NODELAY  ,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的。
                    setnonblocking(clientsock);
                    struct epoll_event ev;
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = clientsock;
                    if(epoll_ctl(epfd, EPOLL_CTL_ADD, clientsock, &ev) == -1){
                        perror("epoll_ctl: listen_sock");
                        exit(EXIT_FAILURE);
                    }
            }
            else{
                /* 
                EPOLLRDHUP (since Linux 2.6.17)
              Stream socket peer closed connection, or shut down writing
              half of connection.  (This flag is especially useful for
              writing simple code to detect peer shutdown when using
              edge-triggered monitoring.)
              */
                if(evs[i].events & EPOLLRDHUP){
                    cout << "1client(eventfd= " << evs[i].data.fd <<") disconnected.\n";
                    close(evs[i].data.fd);
                }
                /* 
                EPOLLPRI
                There is some exceptional condition on the file descriptor.
              Possibilities include:

              •  There is out-of-band data on a TCP socket (see tcp(7)).

              •  A pseudoterminal master in packet mode has seen a state
                 change on the slave (see ioctl_tty(2)).

              •  A cgroup.events file has been modified (see cgroups(7))*/
                else if(evs[i].events & (EPOLLIN | EPOLLPRI)){
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
                else if(evs[i].events & EPOLLOUT){}
                else{
                    cout << "3client(eventfd= " << evs[i].data.fd <<") disconnected.\n";
                    close(evs[i].data.fd);
                }
            }
        }
    }


    return 0;
}