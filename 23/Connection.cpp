#include "Connection.h"
#include <unistd.h>
#include <cstring>



Connection::Connection(EventLoop *loop, Socket *clientsock) : loop_(loop), clientsock_(clientsock)
{
    clientChannel_ = new Channel(clientsock_->fd(), loop_);
    clientChannel_->setReadcallback(std::bind(&Connection::onMessage, this));
    clientChannel_->setClosecallback(std::bind(&Connection::closeCallback, this));
    clientChannel_->setErrorcallback(std::bind(&Connection::errorCallback, this));
    clientChannel_->setWritecallback(std::bind(&Connection::writeCallback, this));
    //clientChannel_->setET();
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
    // printf("client(eventfd=%d) error.\n",fd());
    // close(fd());            // 关闭客户端的fd。
    errorCallback_(this);
}

void Connection::closeCallback()
{
    // std::cout << "client(eventfd= " << fd() <<") disconnected.\n";
    // close(fd());
    closeCallback_(this);
}

void Connection::setCloseCallback(std::function<void(Connection*)> fn)
{
    closeCallback_ = fn;
}

void Connection::setErrorCallback(std::function<void(Connection*)> fn)
{
    errorCallback_ = fn;
}

void Connection::setonMessageCallback(std::function<void(Connection*, std::string)> fn)
{
    onMessageCallback_ = fn;
}

void Connection::setsendCompleteCallback(std::function<void(Connection*)> fn)
{
    sendCompleteCallback_ = fn;
}

void Connection::onMessage(){
    char buffer[1024];
    while(true){
        memset(buffer, 0, sizeof(buffer));
        ssize_t nread = recv(fd(), buffer, sizeof(buffer) ,0);
        //全部数据已经发送完毕
        if((nread < 0) && ((errno == EAGAIN) ||(errno == EWOULDBLOCK))){ //数据已传输完成
            while(true){
            
                uint32_t nlen, len;
                memcpy(&nlen, inputbuffer_.data(), 4);
                len = ntohl(nlen);
                if(inputbuffer_.size() < len + 4) break;
                //printf("recv(eventfd=%d):%s\n", fd(), inputbuffer_.data());
                std::string message(inputbuffer_.data() + 4, len);
                inputbuffer_.erase(0, len + 4);
                ///////////////////////////////////////////////////////
                printf("message (eventfd=%d):%s\n", fd(), message.c_str());
                /*
                //经过若干步的计算，得出outputbuffer
                outputbuffer_ = inputbuffer_;
                //资源交接，inputbuffer_的数据可以不要了
                inputbuffer_.clear();
                send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
                */
                onMessageCallback_(this, message);
            }
            break;
        }

        else if(nread == 0){
            // std::cout << "2client(eventfd= " << fd_ <<") disconnected.\n";
            // close(fd_);
            closeCallback();
            break;
        }
        else if((nread < 0) && (errno == EINTR)) continue;
        else{
            //printf("recv(eventfd=%d):%s\n",fd(),buffer);
            //send(fd(),buffer,strlen(buffer),0);
            inputbuffer_.append(buffer, nread);
        }
    }
}

//使用发送缓冲区发送数据
void Connection::send(const char *data, size_t size)
{
    outputbuffer_.append(data, size);       //把数据读入缓冲区
    clientChannel_->enableWritting();
}

//写事件到达写入缓冲区
void Connection::writeCallback()
{
    size_t writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
    if(writen > 0) outputbuffer_.erase(0, writen);

    //如果用户定义的缓冲区没有数据了，就注销写事件的关注
    if(outputbuffer_.size() == 0) {
        clientChannel_->disableWritting();
        sendCompleteCallback_(this);
    }
}