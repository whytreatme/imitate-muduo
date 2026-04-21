#include "Connection.h"

/*
    class Connection{
private:
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientChannel_;
public:
    Connection(EventLoop *loop, Socket *);
    ~Connection();
};
*/

Connection::Connection(EventLoop *loop, Socket *clientsock) : loop_(loop), clientsock_(clientsock)
{
    clientChannel_ = new Channel(clientsock_->fd(), loop_);
    clientChannel_->setReadcallback(std::bind(&Channel::onMessage, clientChannel_));
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