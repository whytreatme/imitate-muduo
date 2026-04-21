#include "EchoServer.h"
# include <unistd.h>
# include <sys/syscall.h>
/*
class EchoServer{
private:
    TcpServer tcpserver_;
public:
    EchoServer(const std::string &ip, const uint16_t port);
    ~EchoServer();

    void Start();    //启动服务

    //回调作为抓手操作底层类
    void HandleNewConnection(Socket *);
    void HandleMessage(Connection *, std::string);
    void HandleClose(Connection*);
    void HandleError(Connection*);
    void HandleSendComplete(Connection*);
    void HandleTimeout(EventLoop*);
};
*/

EchoServer::EchoServer(const std::string &ip, const uint16_t port, int nums_threads)
    : tcpserver_(ip, port, nums_threads)
{
    tcpserver_.setcloseCallback(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorCallback(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setnewConnectionCallback(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setonMessageCallback(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendCompleteCallback(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    //tcpserver_.settimeOutCallback(std::bind(&EchoServer::HandleTimeout, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}

void EchoServer::Start()
{
    tcpserver_.start();
}

// 处理新客户端连接请求，在TcpServer类中回调此函数。
void EchoServer::HandleNewConnection(Connection *conn)
{
    std::cout << "New Connection Come in." << std::endl;
    printf("EchoServer::HandleNewConnection() thread is %ld.\n",syscall(SYS_gettid));

    // 根据业务的需求，在这里可以增加其它的代码。
}


void EchoServer::HandleMessage(Connection *conn, std::string& msg)
{
    printf("EchoServer::HandleMessage() thread is %ld.\n", syscall(SYS_gettid));
    //经过若干步的计算，得出outputbuffer
    msg = "reply:" + msg;
    conn->send(msg.data(), msg.size());
}

void EchoServer::HandleClose(Connection*)
{
    std::cout << "EchoServer conn closed." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

void EchoServer::HandleError(Connection*)
{
    std::cout << "EchoServer conn error." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

void EchoServer::HandleSendComplete(Connection*)
{
    std::cout << "Message send complete." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}

/*void EchoServer::HandleTimeout(EventLoop*)
{
    std::cout << "EchoServer timeout." << std::endl;

    // 根据业务的需求，在这里可以增加其它的代码。
}*/
