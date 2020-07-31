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



//解散群
void DelGroupC(int Socket)
{
    char SendMsg[1500] = "$delgroup$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        for(i = 0 ; i < 8 ; i++)
        {
            if(SendMsg[i] > 57 || SendMsg[i] < 48)
            {
                printf("\033[31m账号输入出错\033[0m");
                sprintf(SendMsg, "close");
                SendMsg[5] = '\0';
                if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));
                return;
            }
        }
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        printf("\033[32m%s\033[0m", RecvMsg);
    }
}



/* 
  * 下面几个函数涉及群成员的操作
  * status为1：群成员
  * 2：群管理员
  * 
  * 5：申请加入群聊的成员
  */
void JoinGroupC(int Socket)
{
    char SendMsg[1500] = "$joingroup$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        for(i = 0 ; i < 8 ; i++)
        {
            if(SendMsg[i] > 57 || SendMsg[i] < 48)
            {
                printf("\033[31m账号输入出错\033[0m");
                sprintf(SendMsg, "close");
                SendMsg[5] = '\0';
                if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));
                return;
            }
        }
        //检测账号正确性，然后发送到服务器
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        printf("\033[32m%s\033[0m", RecvMsg);
    }
}



// 退群
void ExitGroupC(int Socket)
{
    char SendMsg[1500] = "$exitgroup$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        for(i = 0 ; i < 8 ; i++)
        {
            if(SendMsg[i] > 57 || SendMsg[i] < 48)
            {
                printf("\033[31m账号输入出错\033[0m");
                sprintf(SendMsg, "close");
                SendMsg[5] = '\0';
                if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));
                return;
            }
        }
        //检测账号正确性，然后发送到服务器
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        //接收服务器的结果
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        printf("\033[32m%s\033[0m", RecvMsg);
    }
}



//设置管理员
void SetAdminC(int Socket)
{
    char SendMsg[1500] = "$setadmin$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        for(i = 0 ; i < 8 ; i++)
        {
            if(SendMsg[i] > 57 || SendMsg[i] < 48)
            {
                printf("\033[31m账号输入出错\033[0m");
                sprintf(SendMsg, "close");
                SendMsg[5] = '\0';
                if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));
                return;
            }
        }
        //检测账号正确性，然后发送到服务器
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        //接收服务器的结果
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        if(strcmp(RecvMsg, "请输入提升为管理员的成员账号") == 0)
        {
            printf("\033[32m%s\033[0m", RecvMsg);
            scanf("%s", SendMsg);
            for(i = 0 ; i < 8 ; i++)
            {
                if(SendMsg[i] > 57 || SendMsg[i] < 48)
                {
                    printf("\033[31m账号输入出错\033[0m");
                    sprintf(SendMsg, "close");
                    SendMsg[5] = '\0';
                    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                        my_err("send", __LINE__); 
                    memset(SendMsg, 0, sizeof(SendMsg));
                    return;
                }
            }
            //检测账号正确性，然后发送到服务器
            if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));

            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);
            RecvMsg[res] = '\0'; 
            printf("\033[32m%s\033[0m\n", RecvMsg);
        }
        else
        {
            printf("\033[31m%s\033[0m", RecvMsg);
        }
    }
}


//群踢人
void KickOffC(int Socket)
{
    char SendMsg[1500] = "$kickoff$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        for(i = 0 ; i < 8 ; i++)
        {
            if(SendMsg[i] > 57 || SendMsg[i] < 48)
            {
                printf("\033[31m账号输入出错\033[0m");
                sprintf(SendMsg, "close");
                SendMsg[5] = '\0';
                if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));
                return;
            }
        }
        //检测账号正确性，然后发送到服务器
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        //接收服务器的结果
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        if(strcmp(RecvMsg, "请输入踢出的成员账号") == 0)
        {
            printf("\033[32m%s\033[0m", RecvMsg);
            scanf("%s", SendMsg);
            for(i = 0 ; i < 8 ; i++)
            {
                if(SendMsg[i] > 57 || SendMsg[i] < 48)
                {
                    printf("\033[31m账号输入出错\033[0m");
                    sprintf(SendMsg, "close");
                    SendMsg[5] = '\0';
                    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                        my_err("send", __LINE__); 
                    memset(SendMsg, 0, sizeof(SendMsg));
                    return;
                }
            }
            //检测账号正确性，然后发送到服务器
            if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));

            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);
            RecvMsg[res] = '\0'; 
            printf("\033[32m%s\033[0m\n", RecvMsg);
        }
        else
        {
            printf("\033[31m%s\033[0m", RecvMsg);
        }
    }
}



//查看我的群
void MyGroupsC(int Socket)
{
    char SendMsg[1500] = "$mygroups$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        while(1)
        {
            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);
            RecvMsg[res] = '\0'; 
            if(strcmp(RecvMsg, "No More") != 0)
                printf("\033[32m%s\033[0m", RecvMsg);
            else{
                break;
            }
        }
    }
}


void MyGroupC(int Socket)
{
    char SendMsg[1500] = "$mygroup$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        if(strcmp(RecvMsg, "请输入要查看群成员的群号") == 0)
        {
            printf("\033[32m%s\033[0m", RecvMsg);
            scanf("%s", SendMsg);
            for(i = 0 ; i < 8 ; i++)
            {
                if(SendMsg[i] > 57 || SendMsg[i] < 48)
                {
                    printf("\033[31m账号输入出错\033[0m");
                    sprintf(SendMsg, "close");
                    SendMsg[5] = '\0';
                    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                        my_err("send", __LINE__); 
                    memset(SendMsg, 0, sizeof(SendMsg));
                    return;
                }
            }
            //检测账号正确性，然后发送到服务器
            if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));

            while(1)
            {
                memset(RecvMsg, 0, sizeof(RecvMsg));
                if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                    my_err("recv", __LINE__);
                RecvMsg[res] = '\0'; 
                if(strcmp(RecvMsg, "No More") != 0)
                    printf("\033[32m%s\033[0m", RecvMsg);
                else{
                    break;
                }
            }        
        }
    }
}


//该函数用于查看群聊聊天记录
void GroupChatHistoryC(int Socket)
{
    char SendMsg[1500] = "$groupchathistory$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));


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
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        for(i = 0 ; i < 8 ; i++)
        {
            if(SendMsg[i] > 57 || SendMsg[i] < 48)
            {
                printf("\033[31m账号输入出错\033[0m");
                sprintf(SendMsg, "close");
                SendMsg[5] = '\0';
                if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));
                return;
            }
        }
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        while(1)
        {
            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);
            RecvMsg[res] = '\0'; 

            if(strcmp(RecvMsg, "$close$") == 0)
            {
                printf("\033[33m\t\t没有更多了...\033[0m\n");
                return;
            }
            else
            {
                printf("%s", RecvMsg);
            }
        }
    }
}



//此函数用于收发群聊信息
void GroupChatC(int Socket)
{
    char SendMsg[1500] = "$sendgroupmsg$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));    

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
        printf("\033[32m你想在哪个群发言:\033[0m\n");
        scanf("%s", SendMsg);
        //将此用户发言的群号发送至服务器
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));
        
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';

        if(strcmp(RecvMsg, "\033[31m你不在群中\033[0m\n") == 0)
        {
            printf("%s", RecvMsg);
            return;
        }

        printf("\033[32m%s\033[0m\n", RecvMsg);
        printf("\033[33m输入$close$结束聊天\033[0m\n");

        pthread_t sendthread, recvthread;
        pthread_create(&sendthread, NULL, (void*)Send, (void*) &Socket);
        pthread_create(&recvthread, NULL, (void*)Recv, (void*) &Socket);
        
        void* result;
        pthread_join(sendthread, &result);
        pthread_join(recvthread, &result);

    }
}



void TransmitFileC(int Socket)
{
    char SendMsg[1500] = "$transmitfile$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));    

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
        
    }
}

void AcceptFileC(int Socket)
{
    char SendMsg[1500] = "$acceptfile$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));    

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

    }
}