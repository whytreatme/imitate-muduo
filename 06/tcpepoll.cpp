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
#include "Epoll.h"
#include "Channel.h"

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
   

    
    Epoll ep;
    //ep.addfd(servsock.fd(), EPOLLIN);
    Channel *serveChannel = new Channel(servsock.fd(), &ep);
    serveChannel->enableReading();

    while(true){
        
        std::vector<Channel*> channels = ep.loop();
        
        for(auto& ch : channels){
            if(ch->revents() & EPOLLRDHUP){
                cout << "1client(eventfd= " << ch->fd() <<") disconnected.\n";
                close(ch->fd());
            }
            else if(ch->revents() & (EPOLLIN | EPOLLPRI)){
                if(ch == serveChannel){  
                    InetAddress clientaddr;
                    Socket *clientsock = new Socket(servsock.accept(clientaddr));
                    clientsock->settcpnodelay(true);
                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());
                    //ep.addfd(clientsock->fd(),  EPOLLIN | EPOLLET);
                   Channel *clientChannel = new Channel(clientsock->fd(), &ep);
                   clientChannel->setET();
                   clientChannel->enableReading();
                }
                else{
                    char buffer[1024];
                    while(true){
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t nread = recv(ch->fd(), buffer, sizeof(buffer) ,0);
                        if((nread < 0) && ((errno == EAGAIN) ||(errno == EWOULDBLOCK))) break;
                        else if(nread == 0){
                            cout << "2client(eventfd= " << ch->fd() <<") disconnected.\n";
                            close(ch->fd());
                            break;
                        }
                        else if((nread < 0) && (errno == EINTR)) continue;
                        else{
                            printf("recv(eventfd=%d):%s\n",ch->fd(),buffer);
                            send(ch->fd(),buffer,strlen(buffer),0);
                        }
                    }
                } 
            } 
            else if(ch->revents() & EPOLLOUT){}
            else{
                cout << "3client(eventfd= " << ch->fd() <<") disconnected.\n";
                close(ch->fd());
            }
            
        }
    }


    return 0;
}