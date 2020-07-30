//这是客户端指令判断的部分，应与客户端一起进行编译
#ifndef __CLIENT_C
#define __CLIENT_C
#endif // !__CLIENT_C
#include "global.h"
#include "orders.h"


//用于分析指令
int AnalyseOrder(char* buf, int Socket)
{
    int flag1,flag2,i,j;
    for(i = 0 ; i < 1500 ; i++)
    {
        if(buf[i] == '$')
        {
            flag1 = i;
            for(j = i+1 ; j < 1500; j++)
            {
                if(buf[j] == '$')
                {
                    flag2 = j;
                    break;
                }
            }


            if(buf[j] != '$')
            {
                //有前无后，指令无效
                printf("\033[31m无效的指令，请重新输入:(\033[0m\n");
                fflush(stdin);
                break;
            }
            else
            {
                //找到前后$了，开始执行命令
                int order = JudgeOrder(buf, flag1, flag2, Socket);
                printf("\033[33m命令执行完毕!\033[0m\n");
                fflush(stdin);
                if(order == 0)
                {
                    printf("\033[31m无效的指令，请重新输入:(\033[0m\n");
                    fflush(stdin);
                    break;
                }
            }
            break;
        }
        else
        {
            //没找到前$，指令无效
            printf("\033[31m请输入正确的指令:(\033[0m\n");
            break;
        }
        fflush(stdin);
    }
}


//用于判断指令
int JudgeOrder(char*buf, int flag1, int flag2, int Socket)
{
    int i,j;
    char order[1500];
    //先判断第三种非法输入情况
    if((buf[flag1] == '$') && (buf[flag1+1] == '$'))
    {
        fprintf(stderr, "字符串buf错误，连续输入两个$");
        memset(order, 0, 1500);
        return 0;
    }    

    //将命令复制进来
    strncpy(order, &buf[flag1+1], flag2-flag1-1);


    //这一部分在解析命令，通过字符串的比较，来判断输入的命令指向的下一步操作是什么
    if(strcmp(order, "help") == 0){
        Help();
    }
    else if (strcmp(order, "logup") == 0 || strcmp(order, "signup")==0 || strcmp(order, "enroll")==0 || strcmp(order, "register")==0) {
        SignupC(Socket);
    }
    else if (strcmp(order, "signin") == 0 || strcmp(order, "login") == 0) {
        SigninC(Socket);
    }
    else if (strcmp(order, "logout") == 0 || strcmp(order, "signout") == 0) {
        SignoutC(Socket);
    }
    else if (strcmp(order, "exit") == 0 || strcmp(order, "quit") == 0) {
        Exit(Socket);
    }
    else if (strcmp(order, "myfriends") == 0 || strcmp(order, "friendlist")) {
        MyfriendsC(Socket);
    }
    else if(strcmp(order, "sendmsg") == 0 ) {
        PrivateChatC(Socket);
    }
    else if (strcmp(order, "checkfriend") == 0 || strcmp(order, "myfriend") == 0) {
        CheckFriendC(Socket);
    }
    else if (strcmp(order, "forgetpassword") == 0 || strcmp(order, "forgetpasswd") == 0 || strcmp(order, "findpassword") == 0 || strcmp(order, "findpasswd") == 0) {
        FindPasswordC(Socket);
    }
    else if (strcmp(order, "changepassword") == 0 || strcmp(order, "changepasswd") == 0) {
        ChangePasswordC(Socket);
    }
    else if (strcmp(order, "addfriend") == 0) {
        AddFriendC(Socket);
    }
    else if (strcmp(order, "delfriend") == 0) {
        DelFriendC(Socket);
    }
    else if(strcmp(order, "blockfriend") == 0) {
        BlockFriendC(Socket);
    }
    else if(strcmp(order, "specialcarefriend") == 0 || strcmp(order, "specialcare") == 0) {
        SpecialCareFriendC(Socket);
    }
    else if(strcmp(order, "chathistory") == 0 || strcmp(order, "chatlog") == 0) {
        ChatHistoryC(Socket);
    }
    else if(strcmp(order, "creategroup") == 0) {
        CreateGroupC(Socket);
    }
    else if (strcmp(order, "delgroup") == 0) {
        DelGroupC(Socket);
    }
    else if(strcmp(order, "joingroup") == 0) {
        JoinGroupC(Socket);
    }
    else if(strcmp(order, "exitgroup") == 0 || strcmp(order, "quitgroup") == 0) {
        ExitGroupC(Socket);
    }
    else if(strcmp(order, "setadmin") == 0) {
        SetAdminC(Socket);
    }
    else if (strcmp(order, "kickoff") == 0) {
        KickOffC(Socket);
    }
    else if(strcmp(order, "mygroups") == 0) {
        MyGroupsC(Socket);
    }
    else if(strcmp(order, "mygroup") == 0) {
        MyGroupC(Socket);
    }
    else if(strcmp(order, "groupchathistory") == 0) {
        GroupChatHistoryC(Socket);
    }
    else {
        fprintf(stderr, "无匹配命令");
        memset(order, 0, 1500);
        return 0;
    }
    

    memset(order, 0, 1500);
    return 1;
}


//此命令将所有已实现的功能的txt文本文档打在终端上
void Help(void)
{
    FILE * pFile;
    char ch[256];
    pFile = fopen("help.txt", "r");
    while(!feof(pFile))
    {
        fgets(ch, 256, pFile);
        printf("\t\t\t%s\n", ch);
    }
}


//注册功能
void SignupC(int Socket)
{
    char SendMsg[1500] = "$signup$";
    char RecvMsg[1500];
    signal = 1;//创建信号量之后，请输入昵称就是接受信息的线程最后一次工作，然后就被上锁
    memset(Msg,0, sizeof(Msg));
    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 

    int res;
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    //这里是用户在收到请输入您的昵称这句话之后，在终端上输入创建的账户昵称
    printf("\033[32m%s\033[0m\n", RecvMsg);
    scanf("%s", SendMsg);
    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 

    memset(RecvMsg, 0, sizeof(RecvMsg));
    //向服务器发送昵称后，要等待接收服务器传来的请输入密码指示
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    memset(RecvMsg, 0, sizeof(RecvMsg));
    
    
    strcpy(SendMsg, getpass("请输入密码"));
    strcpy(RecvMsg, getpass("请重复输入密码"));
    char* temp;


    if(strcmp(SendMsg, RecvMsg) != 0 || strlen(SendMsg) >= 20) 
    {
        printf("\033[31m密码输入错误\033[0m\n");
        if(send(Socket, "failed to create", strlen("failed to create"), 0) < 0)
            my_err("send", __LINE__); 
        return;        
    }
    else {
        printf("\033[32m密码输入正确，账户创建中...\033[0m\n");
    }

    encrypt(SendMsg, &temp);
    //检测完输入的两次密码的正确性后，将密码发送到服务器，进行账户建立以及随机生成账号
    if(send(Socket, temp, strlen(temp), 0) < 0)
        my_err("send", __LINE__); 
    memset(temp, 0, sizeof(temp));
    //接收账号
    memset(RecvMsg, 0, sizeof(RecvMsg));
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    printf("\033[32m%s\033[0m\n", RecvMsg);

    signal = 0;
    pthread_mutex_unlock(&mutex);
}


//登录功能
void SigninC(int Socket)
{
    //每个指令都应有这一部分，表示向服务器发送语句，让其知道客户端的请求
    char SendMsg[1500] = "$signin$";
    char RecvMsg[1500];
    signal = 1;
    int i;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));

    //请输入您的账号:
    int res;
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    printf("\033[32m%s\033[0m\n", RecvMsg);
    scanf("%s", SendMsg);

    //对输入的账号进行两次检测
    if(strlen(SendMsg) > 8)
    {
        printf("\033[31m账号非法，请重新输入\033[0m\n");
        strcpy(SendMsg, "账号输入出错");
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));
        return;
    }
    for(i = 0 ; i < 8 ; i++)
    {
        if(SendMsg[i] > 57 || SendMsg[i] < 48)//说明非数字
        {
            printf("\033[31m账号非法，请重新输入\033[0m\n");
            strcpy(SendMsg, "账号输入出错");
             if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));
            return;
        }
    }

    //发送账号至服务器
    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    //接收服务器回馈
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    if(strcmp(RecvMsg, "未找到该账号") == 0)
    {
        printf("\033[31m未找到该账号\033[0m\n");
        return;
    }
    else
    {//输入密码并且发送
        char* temp = NULL;
        memset(temp, 0, sizeof(temp));
        strcpy(SendMsg, getpass(temp));
        encrypt(SendMsg, &temp);
        if(send(Socket, temp, strlen(temp), 0) < 0)
            my_err("send", __LINE__); 
    }

    //接收欢迎信息或者被拒绝访问
    memset(RecvMsg, 0, sizeof(RecvMsg));
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    if(strcmp(RecvMsg, "密码错误，您已被拒绝访问") == 0)
    {
        printf("\033[31m%s\033[0m\n", RecvMsg);
        return ;
    }
    else
    {
        printf("\033[32m%s\033[0m\n", RecvMsg);
    }

    //登陆之后接收离线信息
    RecvDatabaseMsg(Socket);
}


//注销功能
void SignoutC(int Socket)
{
    char SendMsg[1500] = "$signout$";
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
    }
}


//退出程序
void Exit(int Socket)
{
    SignoutC(Socket);
    printf("期待你的下次使用QWQ\n");
    exit(0);
}


//拉取好友列表，对应的数据库表名是friends
void MyfriendsC(int Socket)
{
    char SendMsg[1500] = "$myfriends$";
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
        printf("\033[32m%s[0m\n\n", RecvMsg);
        while(1)
        {  
            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);

            if(strcmp(RecvMsg, "\t\t没有更多了...") == 0)
            {
                printf("\033[32m%s\033[0m\n\n", RecvMsg);
                return;
            }
            else
            {
                printf("%s\n\n", RecvMsg);
            }
        }
        return;
    }
}



//此函数用于收发私聊信息
void PrivateChatC(int Socket)
{
    char SendMsg[1500] = "$sendmsg$";
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
        printf("\033[32m你想和谁私聊:\033[0m\n");
        printf("\033[32mnickname:\033[0m");
        scanf("%s", SendMsg);
        //将被此用户私聊的用户名发送至服务器
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));
        
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';

        if(strcmp(RecvMsg, "\033[31m你必须先加对方为好友，才能发起对话\033[0m\n") == 0)
        {
            printf("%s", RecvMsg);
            return;
        }

        printf("\033[32m%s\033[0m\n", RecvMsg);
        printf("\033[33m输入$close$结束聊天\033[0m\n");

        while(1)
        {
            scanf("%s", SendMsg);
            if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));

            if(strcmp(SendMsg, "$close$") == 0)
                break;
        }
    }
}


//此函数用于在用户登陆之后接收之前的离线信息
void RecvDatabaseMsg(int Socket)
{
    char RecvMsg[1500];
    int res;
    while(1)
    {
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';

        if(strcmp(RecvMsg, "$finished$") == 0)
        {
            printf("\033[33m离线信息接收完毕\033[0m\n");
            break;
        }
        printf("%s", RecvMsg);
    }
}


//查看好友状态：在线情况，我对他的关系
void CheckFriendC(int Socket)
{
    char SendMsg[1500] = "$checkfriend$";
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
    //前面这部分完成了和服务器的互动， 先发送指示，再接收回复


    if(strcmp(RecvMsg, "请先登录")==0)
    {
        printf("\033[31m%s\033[0m\n", RecvMsg);
        return;      
    }
    else
    {
        printf("\033[32m%s\033[0m\n", RecvMsg);
        scanf("%s", SendMsg);
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));   

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';
        printf("\033[32m%s\033[0m\n", RecvMsg);
    }
}


//此函数用于帮助用户找回密码
void FindPasswordC(int Socket)
{
    char SendMsg[1500] = "$findpassword$";    
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
        if(strcmp(RecvMsg, "请以root身份登录")==0)
        {
            printf("\033[31m%s\033[0m\n", RecvMsg);
            return;      
        }
        else
        {
            printf("\033[32m%s\033[0m\n", RecvMsg);
            scanf("%s", SendMsg);    
            if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));    

            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);
            RecvMsg[res] = '\0';
            while(1)
            {
                char* temp = NULL;
                memset(temp, 0, sizeof(temp));
                strcpy(SendMsg, getpass("请输入新密码"));
                strcpy(RecvMsg, getpass("请重复输入新密码"));
                if(strcmp(SendMsg, RecvMsg) == 0)
                {
                    encrypt(SendMsg, &temp);
                    if(send(Socket, temp, strlen(temp), 0) < 0)
                        my_err("send", __LINE__); 
                    break;
                }
                else
                {
                    printf("\033[31m输入错误，请重新输入\033[0m\n");
                }
            }
        }
    }
    
}       



//这个函数用来修改密码
void ChangePasswordC(int Socket)
{
    char SendMsg[1500] = "$changepassword$";    
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
        while(1)
        {
            char* temp = NULL;
            memset(temp, 0, sizeof(temp));
            strcpy(SendMsg, getpass("请输入新密码"));
            strcpy(RecvMsg, getpass("请重复输入新密码"));
            if(strcmp(SendMsg, RecvMsg) == 0)
            {
                encrypt(SendMsg, &temp);
                if(send(Socket, temp, strlen(temp), 0) < 0)
                    my_err("send", __LINE__); 
                break;
            }
            else
            {
                printf("\033[31m输入错误，请重新输入\033[0m\n");
            }
        }
    }
}



//下面四个函数分别对应 加好友、删好友、拉黑、特别关心
void AddFriendC(int Socket)
{
    char SendMsg[1500] = "$addfriend$";    
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
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));    

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';
    }
}


void DelFriendC(int Socket)
{
    char SendMsg[1500] = "$delfriend$";    
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
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));   

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
    }    
}


void BlockFriendC(int Socket)
{
    char SendMsg[1500] = "$blockfriend$";    
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
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';     
    }    
}


void SpecialCareFriendC(int Socket)
{
    char SendMsg[1500] = "$specialcarefriend$";    
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
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));    

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
    }    
}


//该函数用于查找聊天记录
void ChatHistoryC(int Socket)
{
    char SendMsg[1500] = "$ChatHistory$";    
    char RecvMsg[1500];
    signal = 1;
    int i, res;
    memset(Msg, 0, sizeof(Msg));

    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));    

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
        if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));    

        while(1)
        {
            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);
            RecvMsg[res] = '\0'; 
            if(strcmp(RecvMsg, "$finish$") == 0)
                break;
            printf("%s", RecvMsg);
        }
    } 
}