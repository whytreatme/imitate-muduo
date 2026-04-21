#include "TcpServer.h"
#include "Channel.h"
#include <functional>

/*
class TcpServer{
private:
    EventLoop loop_;
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();
    void start();
};
*/

TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    Socket *servsock = new Socket(create_non_block());
   
    servsock->setreuseaddr(true);
    servsock->setreuseport(true);
   
    InetAddress servaddr(ip, port);
    servsock->bind(servaddr);
    servsock->listen();
   

    
    //Epoll ep;
    //EventLoop loop;
    Channel *serveChannel = new Channel(servsock->fd(), &loop_);
    serveChannel->setReadcallback(std::bind(&Channel::newConnection, serveChannel, servsock));
    serveChannel->enableReading();
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    loop_.run();
}