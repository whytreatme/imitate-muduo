// 网络通讯的客户端程序。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "protocol.h"
uint32_t onSend(void *, void*);

int main(int argc, char *argv[])
{
    
    if (argc != 3)
    {
        printf("usage:./client ip port\n"); 
        printf("example:./client 192.168.38.128 5005\n\n"); 
        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char payload[1024], buf[1024];
 
    if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) { printf("socket() failed.\n"); return -1; }
    
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed.\n",argv[1],argv[2]); close(sockfd);  return -1;
    }

    printf("connect ok.\n");
    // printf("开始时间：%d",time(0));

    for (int ii=0;ii<1000;ii++)
    {
        // 从命令行输入内容。
        memset(payload,0,sizeof(payload));
       // printf("please input:"); scanf("%s",buf);
        sprintf(payload, "这是第%d个猪狗不如的傻逼。", ii);
        // int len = strlen(buf);
        // char tmp[1024];
        // memcpy(tmp, &len, 4);
        // memcpy(tmp + 4, buf, len);
        uint32_t len = onSend(payload, buf);
       
        if (send(sockfd, buf, len + HEADER_SIZE, 0) <=0)       // 把命令行输入的内容发送给服务端。
        { 
            printf("write() failed.\n");  close(sockfd);  return -1;
        }
        //usleep(100);
        // memset(buf,0,sizeof(buf));
        // if (recv(sockfd,buf,sizeof(buf),0) <=0)      // 接收服务端的回应。
        // { 
        //     printf("read() failed.\n");  close(sockfd);  return -1;
        // }

        // printf("recv:%s\n",buf);
    }

    // printf("结束时间：%d",time(0));
} 

uint32_t onSend(void *payload, void *buf)
{
    uint32_t len = strlen((char*)payload);
    uint32_t nlen = htonl(len);
    memcpy(buf, &nlen, HEADER_SIZE);
    memcpy(buf + HEADER_SIZE, payload, len);
    return len;
}