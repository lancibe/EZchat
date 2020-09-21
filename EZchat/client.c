// 基于C/S模型的客户端
// gcc client.c log.c myerr.c orders.c orders2.c ase.c -o client -lpthread -lcrypto -ldl
#ifndef __CLIENT_C
#define __CLIENT_C
#endif
#include "global.h"

//创建两个线程，分别用于收发
pthread_t SendThread, RecvThread;

void* executeSend(void);
void* executeRecv(void);

//两个线程之间通信变量
char SendMsg[1500] = {0};
char RecvMsg[1500] =  {0};


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
    ServerMsg.sin_addr.s_addr = inet_addr("192.168.30.112");//注意这里的端口号和IP地址一定都是服务器的
    if(connect(server, (struct sockaddr*)&ServerMsg, sizeof(struct sockaddr_in)) < 0)
        my_err("connect", __LINE__);


    //从服务器接收欢迎信息
    int res;
    if((res = recv(server, RecvMsg, sizeof(RecvMsg)-1 , 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    printf("\033[32m%-s\033[0m\n", RecvMsg);
    printf("\033[33m请输入$help$打开帮助文档:D\033[0m\n");


    void* result;
    //使两个线程开始工作
    if(0 > pthread_create(&SendThread, 0, (void*) executeSend, NULL))
        my_err("pthread_create", __LINE__);
    if(0 > pthread_join(SendThread, &result))
        my_err("pthread_join", __LINE__);







    return 0;
}


//线程执行函数
void * executeSend(void)
{
    int i,j;
    int flag1,flag2;
    while(1)
    {
        //向服务器发送信息
        static char buf[256];
        memset(buf, 0, sizeof(buf));
        printf("\033[33m请输入命令:  \033[0m");
        scanf("%s", buf);
        AnalyseOrder(buf, server);
    }
}