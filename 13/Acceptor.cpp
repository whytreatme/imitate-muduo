#include "Acceptor.h"

/*
class Acceptor{
private:
    EventLoop *loop_;
    Socket *servsock_;
    Channel *acceptChannel_;
public:
    Acceptor(EventLoop *loop, const std::string &ip, uint16_t port);
    ~Acceptor();
};
*/

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, uint16_t port) : loop_(loop)
{
    servsock_ = new Socket(create_non_block());
   
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
   
    InetAddress servaddr(ip, port);
    servsock_->bind(servaddr);
    servsock_->listen();
   
    acceptChannel_ = new Channel(servsock_->fd(), loop_);
    acceptChannel_->setReadcallback(std::bind(&Channel::newConnection, acceptChannel_, servsock_));
    acceptChannel_->enableReading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptChannel_;
}