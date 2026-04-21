/*
* 我的回显服务器
*/
#include <iostream>
#include "EchoServer.h"
using namespace std;


int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "Usage " << argv[0] << " ip port\n";
        cout << "./echoserver 192.168.38.128 5005\n\n";
        return -1;
    }

    

    EchoServer echoserver(argv[1], atoi(argv[2]));
    echoserver.Start();
   

    return 0;
}