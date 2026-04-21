#include "TcpServer.h"
#include "Channel.h"
#include <functional>



TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{   
   
   acceptor_ =  new Acceptor(&loop_, ip, port);
   acceptor_->setnewConnection(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;

    for(auto &aa : conns_)
    {
        delete aa.second;
    }
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newConnection(Socket *clientsock)
{
    Connection* conn = new Connection(&loop_, clientsock);
     printf ("new Connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    conns_[conn->fd()] = conn; 
}