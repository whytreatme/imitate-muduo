#include "TcpServer.h"
#include "Channel.h"
#include <functional>



TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{   
   acceptor_ =  new Acceptor(&loop_, ip, port);
   acceptor_->setnewConnection(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
    loop_.setepollTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
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
    conn->setCloseCallback(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn->setErrorCallback(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn->setonMessageCallback(std::bind(&TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendCompleteCallback(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));
    //printf ("new Connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    conns_[conn->fd()] = conn; 
    if(newConnectionCallback_) newConnectionCallback_(conn);
}

void TcpServer::errorConnection(Connection *conn)
{
    if(errorCallback_) errorCallback_(conn);
    //printf("client(eventfd=%d) error.\n",conn->fd());
    //close(fd());            // 关闭客户端的fd。
    conns_.erase(conn->fd());
    delete conn;
}

//将接受到的报文计算并发回
void TcpServer::onMessage(Connection *conn, std::string& msg)
{
    if(onMessageCallback_) onMessageCallback_(conn, msg);
    /*
    //经过若干步的计算，得出outputbuffer
    msg = "reply:" + msg;
    uint32_t len = msg.size();         //计算报文头部的大小
    uint32_t nlen = htonl(len);       //把报文头部换网络字节序（数字必须）
    std::string tmpbuf((char*)&nlen, 4);   //把转换后的头部填充回报文
    tmpbuf.append(msg);                //把内容填充回报文
    //send(conn->fd(), tmpbuf.data(), tmpbuf.size(), 0);
    conn->send(tmpbuf.data(), tmpbuf.size());
    */
}

void TcpServer::closeConnection(Connection *conn)
{
    if(closeCallback_) closeCallback_(conn);
    //std::cout << "client(eventfd= " << conn->fd() <<") disconnected.\n";
    //close(fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::sendComplete(Connection *conn)
{   
    if(sendComleteCallback_) sendComleteCallback_(conn);
    //printf("send complete.\n");
    
    //根据业务需求增加代码
}

void TcpServer::epollTimeout(EventLoop *loop)
{
    if(timeOutCallback_) timeOutCallback_(loop);
    //printf("epoll time out.\n");

    //根据业务需求增加代码
}

void TcpServer::setnewConnectionCallback(std::function<void (Connection*)> fn)
{
    newConnectionCallback_ = fn;
}

void TcpServer::setonMessageCallback(std::function<void (Connection*, std::string&)> fn)
{
    onMessageCallback_ = fn;
}

void TcpServer::setcloseCallback(std::function<void (Connection*)> fn)
{
    closeCallback_ = fn;
}   

void TcpServer::seterrorCallback(std::function<void (Connection*)> fn)
{
    errorCallback_ = fn;
}  

void TcpServer::setsendCompleteCallback(std::function<void (Connection*)> fn)
{
    sendComleteCallback_ = fn;
}  

void TcpServer::settimeOutCallback(std::function<void (EventLoop*)> fn)
{
    timeOutCallback_ = fn;
}