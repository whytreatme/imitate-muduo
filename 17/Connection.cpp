#include "Connection.h"
#include <unistd.h>



Connection::Connection(EventLoop *loop, Socket *clientsock) : loop_(loop), clientsock_(clientsock)
{
    clientChannel_ = new Channel(clientsock_->fd(), loop_);
    clientChannel_->setReadcallback(std::bind(&Channel::onMessage, clientChannel_));
    clientChannel_->setClosecallback(std::bind(&Connection::closeCallback, this));
    clientChannel_->setErrorcallback(std::bind(&Connection::errorCallback, this));
    clientChannel_->setET();
    clientChannel_->enableReading();
}

Connection::~Connection()
{
    delete clientChannel_;
    delete clientsock_;
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}


void Connection::errorCallback()
{
    printf("client(eventfd=%d) error.\n",fd());
    close(fd());            // 关闭客户端的fd。
}

void Connection::closeCallback()
{
    std::cout << "client(eventfd= " << fd() <<") disconnected.\n";
    close(fd());
}