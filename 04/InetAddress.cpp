#include "InetAddress.h" 

/*class InetAddress{
    private:
        struct sockaddr_in addr_;
    public:
        InetAddress(const std::string ip, uint16_t port);
        InetAddress(const struct sockaddr_in addr);
        const char* ip();
        const uint16_t port();
        const struct sockaddr* addr(); 

};*/ 

InetAddress::InetAddress(const std::string ip, uint16_t port){
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}  

InetAddress::InetAddress(const struct sockaddr_in addr) : addr_(addr)
{

}

InetAddress::InetAddress(){}

const char*InetAddress::ip() const{
    return inet_ntoa(addr_.sin_addr);
}

const uint16_t InetAddress::port() const{
    return ntohs(addr_.sin_port);
}

const struct sockaddr* InetAddress::addr() const{
    return (struct sockaddr*)&addr_;
} 

void InetAddress::setaddr(const InetAddress& old){
    addr_ = old.addr_;
}