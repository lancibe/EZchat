#ifndef __CLIENT_C
#define __CLIENT_C
#endif // !__CLIENT_C
#include "global.h"
#include "orders.h"

//这一部分仍是基于orders.c对用户输入的指令进行分析，并向客户端发送指令
//orders2.c针对于群的操作

//创建群聊
void CreateGroupC(int Socket)
{
    char SendMsg[1500] = "$creategroup$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));

    //接收“请输入群名称”
    int res;
    memset(RecvMsg, 0, sizeof(RecvMsg));
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0'; 

    if(strcmp(RecvMsg, "请先登录")==0)
    {
        printf("\033[31m%s\033[0m\n", RecvMsg);
        return;      
    }
    else
    {
        //发送群名称
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 

        printf("\033[32m群账号是%s\033[0m", RecvMsg);
        
    }
    
}