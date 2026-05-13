/*
* 我的回显服务器
*/
#include <iostream>
#include "EchoServer.h"
using namespace std;


int main(int argc, char *argv[]){
    if(argc != 5){
        cout << "Usage " << argv[0] << " ip port timerIntervalSec idleTimeoutSec\n";
        cout << "./main 0.0.0.0 5005 60 180\n\n";
        return -1;
    }

    

    EchoServer echoserver(argv[1], atoi(argv[2]), 3, 5, atoi(argv[3]), atoi(argv[4]));
    echoserver.Start();
   

    return 0;
}