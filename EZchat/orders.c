//这是客户端指令判断的部分，应与客户端一起进行编译
#ifndef __CLIENT_C
#define __CLIENT_C
#endif // !__CLIENT_C
#include "global.h"


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
    else if (strcmp(order, "logup") == 0 || strcmp(order, "signup")==0) {
        SignupC(Socket);
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


//此命令将打开注册界面
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

    if(strcmp(SendMsg, RecvMsg) != 0 || strlen(SendMsg) >= 20) {
        printf("\033[32m密码输入出错\033[0m\n");
        if(send(Socket, "failed to create", strlen("failed to create"), 0) < 0)
            my_err("send", __LINE__); 
        return;        
    }
    else {
        printf("\033[32m密码输入正确，账户创建中...\033[0m\n");
    }


    //检测完输入的两次密码的正确性后，将密码发送到服务器，进行账户建立以及随机生成账号
    if(send(Socket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    //接收账号
    memset(RecvMsg, 0, sizeof(RecvMsg));
    if((res = recv(Socket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    printf("\033[32m%s\033[0m\n", RecvMsg);

    signal = 0;
    pthread_mutex_unlock(&mutex);
}