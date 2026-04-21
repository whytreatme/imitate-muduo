#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include <string>
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include "ThreadPool.h"


/*
TcpServer有什么权限
TcpServer拥有Acceptor类和管理Connection类的权限
TcpServer拥有Acceptor创建和析构的权限
TcpServer“逻辑上”拥有Connection创建和析构的权限，实际上Connection由EventLoop和Channel协助完成
TcpServer负责管理所有的Connection对象
TcpServer负责处理新连接的回调
TcpServer负责处理连接的关闭和错误
TcpServer负责启动事件循环
*/
class TcpServer{
private:
    EventLoop *mainloop_;               //主线程运行主事件循环
    std::vector<EventLoop*> subloops_;   //线程池运行从事件循环
    thpool *threadpool_;                //建立线程池
    int nums_threads;                   //线程池的线程数量

    Acceptor *acceptor_;
    std::map<int, Connection*>conns_;
    //给echoserver使用的回调函数接口
    std::function<void (Connection*)> newConnectionCallback_;
    std::function<void (Connection*, std::string&)> onMessageCallback_;
    std::function<void (Connection*)> closeCallback_;
    std::function<void (Connection*)> errorCallback_;
    std::function<void (Connection*)> sendComleteCallback_;
    std::function<void (EventLoop*)> timeOutCallback_;
    
public:
    TcpServer(const std::string &ip, const uint16_t port, int numsthreads = 3);
    ~TcpServer();
    void start();

    void newConnection(Socket *);
    void onMessage(Connection *, std::string&);
    void closeConnection(Connection*);
    void errorConnection(Connection*);
    void sendComplete(Connection*);
    void epollTimeout(EventLoop*);

    void setnewConnectionCallback(std::function<void (Connection*)> fn);
    void setonMessageCallback(std::function<void (Connection*, std::string&)> fn);
    void setcloseCallback(std::function<void (Connection*)> fn);
    void seterrorCallback(std::function<void (Connection*)> fn);
    void setsendCompleteCallback(std::function<void (Connection*)> fn);
    void settimeOutCallback(std::function<void (EventLoop*)> fn);
};