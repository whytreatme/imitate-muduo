#include "TcpServer.h"
#include "Channel.h"
#include <functional>
#include "Connection.h"


TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{   
   
   acceptor_ =  new Acceptor(&loop_, ip, port);
   acceptor_->setnewConnection(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newConnection(Socket *clientsock)
{
    Connection* conn = new Connection(&loop_, clientsock);
}