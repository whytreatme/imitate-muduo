#include "TcpServer.h"
#include "Channel.h"
#include <functional>




TcpServer::TcpServer(const std::string &ip, const uint16_t port, int numsthreads) 
            : nums_threads(numsthreads), threadpool_(numsthreads, "IO")
{
   mainloop_ = new EventLoop; 
   mainloop_->setepollTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));  
   
   acceptor_ =  new Acceptor(mainloop_, ip, port);
   acceptor_->setnewConnection(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));

   //创建从事件循环
   for(int i = 0; i < nums_threads; i++)
   {
        subloops_.push_back(new EventLoop);
        subloops_[i]->setepollTimeoutCallback(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));  
        threadpool_.addTask(" EventLoop", std::bind(&EventLoop::run, subloops_[i]));
   }
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    delete mainloop_;

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
    mainloop_->run();
}

void TcpServer::newConnection(Socket *clientsock)
{
    printf("接收到新的连接。\n");
    //Connection* conn = new Connection(mainloop_, clientsock);
    //把Connection分配给从事件循环
    spConnection conn = std::make_shared<Connection>(Connection(subloops_[clientsock->fd() % nums_threads], clientsock));
    conn->setCloseCallback(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn->setErrorCallback(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn->setonMessageCallback(std::bind(&TcpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendCompleteCallback(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));
    //printf ("new Connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    conns_[conn->fd()] = conn; 
    if(newConnectionCallback_) newConnectionCallback_(conn);
}

void TcpServer::errorConnection(spConnection conn)
{
    if(errorCallback_) errorCallback_(conn);
    //printf("client(eventfd=%d) error.\n",conn->fd());
    //close(fd());            // 关闭客户端的fd。
    conns_.erase(conn->fd());
    //delete conn;
}

//将接受到的报文计算并发回
void TcpServer::onMessage(spConnection conn, std::string& msg)
{
    if(onMessageCallback_) onMessageCallback_(conn, msg);
  
}

void TcpServer::closeConnection(spConnection conn)
{
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

void TcpServer::settimeOutCallback(std::function<void (EventLoop*)> fn)
{
    timeOutCallback_ = fn;
}