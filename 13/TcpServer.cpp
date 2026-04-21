#include "TcpServer.h"
#include "Channel.h"
#include <functional>


TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{   
    /*
    Socket *servsock = new Socket(create_non_block());
   
    servsock->setreuseaddr(true);
    servsock->setreuseport(true);
   
    InetAddress servaddr(ip, port);
    servsock->bind(servaddr);
    servsock->listen();
   
    Channel *serveChannel = new Channel(servsock->fd(), &loop_);
    serveChannel->setReadcallback(std::bind(&Channel::newConnection, serveChannel, servsock));
    serveChannel->enableReading();
    */
   acceptor_ =  new Acceptor(&loop_, ip, port);
}

TcpServer::~TcpServer()
{
    delete acceptor_;
}

void TcpServer::start()
{
    loop_.run();
}