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
#include <functional>
#include "InetAddress.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"

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
   

    
    //Epoll ep;
    EventLoop loop;
    Channel *serveChannel = new Channel(servsock.fd(), &loop);
    serveChannel->setReadcallback(bind(&Channel::newConnection, serveChannel, &servsock));
    serveChannel->enableReading();

    loop.run();
   

    return 0;
}