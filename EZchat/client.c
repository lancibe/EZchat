// 基于C/S模型的客户端
// gcc client.c log.c myerr.c -o client -lpthread -ldl
#define __CLIENT_C
#include "global.h"

//创建两个线程，分别用于收发
pthread_t SendThread, RecvThread;

void* executeSend(int);
void* executeRecv(int);

//两个线程之间通信变量
char Msg[1500];
char SendMsg[1500] = {0};
char RecvMsg[1500] = {0};

//服务器套接字
int server;

int main(int argc, char** argv)
{
    //创建一个TCP套接字
    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0)
        my_err("socket", __LINE__);

    //连接服务器
    struct sockaddr_in ServerMsg;
    ServerMsg.sin_family = AF_INET;
    ServerMsg.sin_port = htons(14233);
    ServerMsg.sin_addr.s_addr = inet_addr("192.168.31.136");//注意这里的端口号和IP地址一定都是服务器的
    if(connect(server, (struct sockaddr*)&ServerMsg, sizeof(struct sockaddr_in)) < 0)
        my_err("connect", __LINE__);



    void* result;
    //使两个线程开始工作
    if(0 != pthread_create(&SendThread, 0, (void*) executeSend, NULL))
        my_err("pthread_create", __LINE__);
    if(0 != pthread_create(&RecvThread, 0, (void*) executeRecv, NULL))
        my_err("pthread_create", __LINE__);
    if(0 > pthread_join(RecvThread, &result))
        my_err("pthread_join", __LINE__);
    if(0 > pthread_join(SendThread, &result))
        my_err("pthread_join", __LINE__);



    




    return 0;
}


//线程执行函数
void * executeSend(int Socket)
{
    while(1)
    {
        //向服务器发送信息
        char buf[1500];
        memset(buf, 0, sizeof(buf));
        scanf("%s", buf);
        sprintf(SendMsg, "\033[34m%s\033[0m\n", buf);
        if(send(server, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__);
    }

}

void * executeRecv(int Socket)
{
    //从服务器接收欢迎信息
    int res;
    if((res = recv(server, RecvMsg, sizeof(RecvMsg)-1 , 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    printf("\033[32m%s\033[0m\n", RecvMsg);
    while(1)
    {

    }
}