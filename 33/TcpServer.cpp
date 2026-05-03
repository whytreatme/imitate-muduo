#include "TcpServer.h"
#include "Channel.h"
#include <functional>
#include "Logger.h"




TcpServer::TcpServer(const std::string &ip, const uint16_t port, int numsthreads) 
            : mainloop_(), nums_threads(numsthreads), threadpool_(nums_threads, "IO"),
              acceptor_(mainloop_, ip, port)
{
   mainloop_.setepollTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));  
   acceptor_.setnewConnection(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));

   //创建从事件循环
   for(int i = 0; i < nums_threads; i++)
   {
        subloops_.emplace_back(new EventLoop);
        subloops_[i]->setepollTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));  
        threadpool_.addTask(" EventLoop", std::bind(&EventLoop::run, subloops_[i].get()));
   }
}

TcpServer::~TcpServer()
{
    //delete acceptor_;
    //delete mainloop_;

    // for(auto &aa : conns_)
    // {
    //     delete aa.second;
    // }  

    // for(auto &el : subloops_)
    // {
    //     delete el;
    // }
}

void TcpServer::start()
{
    mainloop_.run();
}

void TcpServer::newConnection(std::unique_ptr<Socket> clientsock)
{
    LOG("TcpServer::newConnection - Handing new connection for fd=%d", clientsock->fd());
    //printf("接收到新的连接。\n");
    //Connection* conn = new Connection(mainloop_, clientsock);
    //把Connection分配给从事件循环
    spConnection conn = std::make_shared<Connection>(*subloops_[clientsock->fd() % nums_threads], std::move(clientsock));
    LOG("TcpServer::newConnection - Setting callbacks for fd=%d", conn->fd());
    conn->setCloseCallback(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn->setErrorCallback(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn->setonMessageCallback(std::bind(&TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendCompleteCallback(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));
    //printf ("new Connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    LOG("TcpServer::newConnection - Callbacks set for fd=%d", conn->fd());
    conns_[conn->fd()] = conn; 
    if(newConnectionCallback_) {
        LOG("TcpServer::newConnection - Invoking application-level new connection callback for fd=%d", conn->fd());
        newConnectionCallback_(conn);
    }

    conn->ConnectEstablished();
}

void TcpServer::errorConnection(spConnection conn)
{
    LOG("TcpServer::errorConnection - Connection error for fd=%d", conn->fd());
    if(errorCallback_) errorCallback_(conn);
    //printf("client(eventfd=%d) error.\n",conn->fd());
    //close(fd());            // 关闭客户端的fd。
    conns_.erase(conn->fd());
    //delete conn;
}

//将接受到的报文计算并发回
void TcpServer::onMessage(spConnection conn, std::string& msg)
{
    LOG("TcpServer::onMessage - Received message from fd=%d, passing to application callback.", conn->fd());
    //printf("我现在在TcpServer::onMessage里！\n");
    if(onMessageCallback_) onMessageCallback_(conn, msg);
  
}

void TcpServer::closeConnection(spConnection conn)
{
    LOG("TcpServer::closeConnection - Closing connection for fd=%d", conn->fd());
    if(closeCallback_) closeCallback_(conn);
    //std::cout << "client(eventfd= " << conn->fd() <<") disconnected.\n";
    //close(fd());
    conns_.erase(conn->fd());
    //delete conn;
}

void TcpServer::sendComplete(spConnection conn)
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

void TcpServer::setnewConnectionCallback(std::function<void (spConnection)> fn)
{
    newConnectionCallback_ = fn;
}

void TcpServer::setonMessageCallback(std::function<void (spConnection, std::string&)> fn)
{
    onMessageCallback_ = fn;
}

void TcpServer::setcloseCallback(std::function<void (spConnection)> fn)
{
    closeCallback_ = fn;
}   

void TcpServer::seterrorCallback(std::function<void (spConnection)> fn)
{
    errorCallback_ = fn;
}  

void TcpServer::setsendCompleteCallback(std::function<void (spConnection)> fn)
{
    sendComleteCallback_ = fn;
}  

void TcpServer::settimeOutCallback(std::function<void(EventLoop*)> fn)
{
    timeOutCallback_ = fn;
}