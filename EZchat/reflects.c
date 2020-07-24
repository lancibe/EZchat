//这是服务端对命令进行反映的文件，应与服务端文件一起进行编译
#include "global.h"
#include "reflects.h"




//分析客户端发来的信息
int Analyse(char* buf, int ClientSocket)
{
    int i,j,flag1,flag2;
    for(i = 0 ; i < 1500 ; i++)
    {
        if(buf[i] == '$')
        {
            flag1 = i;
            for(j = i+1 ; j < 1500 ; j++)
            {
                if(buf[j] == '$')
                {
                    flag2 = j;
                    break;
                }
            }

            if(buf[j] != '$')
            {
                fprintf(stderr, "客户端输入出错");
                fflush(stdin);
                break;
            }
            else
            {
                int reflect = Reflect(buf, flag1, flag2, ClientSocket);


            }
            break;
        }
        else
        {
            //没有前$默认为发送信息到上一个msgto的用户



            break;
        }
        fflush(stdin);
    }   
}




int Reflect(char*buf, int flag1, int flag2, int ClientSocket)
{
    int i,j;
    char reflect[1500];
    if((buf[flag1] == '$') && (buf[flag1+1] == '$'))
    {
        fprintf(stderr, "字符串buf错误，连续输入两个$");
        memset(reflect, 0, 1500);
        return 0;
    }    

    //复制一下buf，去掉前后两个$$
    strncpy(reflect, &buf[flag1+1], flag2- flag1 - 1);

    //开始解析命令
    if(strcmp(reflect, "signup") == 0) {
        Signup(ClientSocket);
    }
    else if(0) {

    }
}


int Signup(int ClientSocket)
{
    char SendMsg[1500]="请输入您的昵称:)";
    char RecvMsg[1500];
    char nickname[21], passwd[21], count[9];
    int len,i,j;

    MYSQL mysql = Connect_Database();

    //向用户发送请输入昵称，并立即进入阻塞，等待接收用户输入的昵称
    if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    if((len = recv(ClientSocket, RecvMsg, sizeof(RecvMsg)-1, 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[len] = '\0';
    memset(nickname,0, sizeof(nickname));
    strncpy(nickname, RecvMsg, 20); 

    //收到了用户发来的昵称后，要发出请输入密码的指示并等待经验证后的
    memset(SendMsg, 0, sizeof(SendMsg));
    strcpy(SendMsg, "请输入密码");
    if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)//上一个recv已经收到了昵称，这里应该主动发送指示，让客户端进行输入
        my_err("send", __LINE__);
    
    
    if((len = recv(ClientSocket, RecvMsg, sizeof(RecvMsg)-1, 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[len] = '\0';

    memset(passwd, 0, sizeof(passwd));
    strncpy(passwd, RecvMsg, 20);
    passwd[20] = '\0';

    if(strcmp(passwd, "failed to create") == 0){
        fprintf(stderr, "密码创建失败\n");
        return 0;
    }

    else 
    {//这时服务器接收到了客户端传来的合法的密码，随机创建一个账户，将其加入到数据库中，且将账号发送给客户
        char Nickname[21];
        strcpy(Nickname, nickname);
        Nickname[20] = '\0';
        while(1)
        {
            Random(count);//这一部分是为客户创建随机数账号
            if(NULL == FindSameCount(count, mysql))
            {
                count[8] = '\0';
                memset(SendMsg,0,sizeof(SendMsg));
                sprintf(SendMsg, "注册成功！你的账号是：%s", count);
                
                struct sockaddr_in addr;
                int length = sizeof(addr);
                getpeername(ClientSocket, (struct sockaddr*)&addr, &length);
                printf("%s创建了账号%s,用户名%s,密码%s\n", inet_ntoa(addr.sin_addr), count, Nickname, passwd);
                
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                break;
            }
                
            else{
                printf("账号%s已存在，正在重新创建\n",count);
            }
        }
        //将用户数据插入数据库
        int Count = atoi(count);
        char res[256];
	    sprintf(res, "insert into userinfo values(default,'%s',%d,'%s')", Nickname, Count, passwd);
        if(!mysql_query(&mysql, res))
            my_err("mysql_query", __LINE__);    
        else {  
            printf("已成功将数据写入数据库\n");
        }
        printf("%s\n", res);


        

    }
}







