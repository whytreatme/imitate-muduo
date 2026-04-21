#include "Acceptor.h"



Acceptor::Acceptor(EventLoop *loop, const std::string &ip, uint16_t port) : loop_(loop)
{
    servsock_ = new Socket(create_non_block());
   
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
   
    InetAddress servaddr(ip, port);
    servsock_->bind(servaddr);
    servsock_->listen();
   
    acceptChannel_ = new Channel(servsock_->fd(), loop_);
    acceptChannel_->setReadcallback(std::bind(&Acceptor::newConnection, this));
    acceptChannel_->enableReading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptChannel_;
}

//#include "Connection.h"
void Acceptor::newConnection(){
    InetAddress clientaddr;
    Socket *clientsock = new Socket(servsock_->accept(clientaddr));
    clientsock->settcpnodelay(true);
    //printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());
 
    //Connection* conn = new Connection(loop_, clientsock);
    newConnectioncb_(clientsock);
}

void Acceptor::setnewConnection( std::function<void(Socket*)> fn)
{
    newConnectioncb_ = fn;
}