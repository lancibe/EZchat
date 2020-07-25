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
    else if(strcmp(reflect, "signin") == 0) {
        Signin(ClientSocket);
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
	    sprintf(res, "insert into userinfo values(default,'%s',%d,'%s',0,-1)", Nickname, Count, passwd);
        if(!mysql_query(&mysql, res))
            my_err("mysql_query", __LINE__);    
        else {  
            printf("已成功将数据写入数据库\n");
        }
    }

    Close_Database(mysql);
}


void Signin(int ClientSocket)
{
    char SendMsg[1500]="请输入您的账号:";
    char RecvMsg[1500];
    int len,i,j, flag;

    MYSQL mysql = Connect_Database();

    if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
        my_err("send", __LINE__); 
    memset(SendMsg, 0, sizeof(SendMsg));

    int res;
    //接收客户端发来的账号
    if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    if(strcmp(RecvMsg, "账号输入出错") == 0)//接收服务器发来的错误信息
        return;
    
    struct sockaddr_in addr;
    int length = sizeof(addr);
    getpeername(ClientSocket, (struct sockaddr*)&addr, &length);

    //匹配账号
    char temp[256];
        //这是为了删去字符串前面的0，防止匹配时出错
    for(i = 0 ; i < res ; i++)
    {
        if(RecvMsg[i] == '0')
            continue;
        else break;
    }
    sprintf(temp, "select nickname,count,passwd from userinfo where count = %s", &RecvMsg[i]);
    char nickname[21], passwd[21], count[9];
    flag = mysql_query(&mysql, temp);//这个函数查找成功就返回0，不管查找出来的是什么结果，只有出错才是非0
    if(flag)//查找出错
    {
        my_err("mysql_qeury", __LINE__);
    }
    else//查找没问题，然后要开始匹配是否有此账号
    {
        MYSQL_RES           *result = NULL;
        MYSQL_ROW           row;
        MYSQL_FIELD         *field;
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(nickname, row[0]);
            strcpy(count, row[1]);
            strcpy(passwd, row[2]);
        }
        if(strcmp(&RecvMsg[i], count) == 0)//查找到该账号
        {
            printf("%s正在尝试从%s登录\n", &RecvMsg[i], inet_ntoa(addr.sin_addr));
            strcpy(SendMsg, "请输入密码:");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__);
            memset(SendMsg, 0, sizeof(SendMsg));
        }
        else//查找失败
        {
            strcpy(SendMsg, "未找到该账号");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));
            return; 
        }
        


        //接收客户端发来的密码
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';       

        //将从客户端接收来的密码和数据库进行比对
        if(strcmp(RecvMsg, passwd) == 0)
        {
            //认证通过
            sprintf(SendMsg, "%s,欢迎访问EZchat", nickname);
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__);
            memset(SendMsg, 0, sizeof(SendMsg));
            printf("%s已经成功从%s登录\n", count, inet_ntoa(addr.sin_addr));
        }
        else
        {
            sprintf(SendMsg, "密码错误，您已被拒绝访问");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__);
            memset(SendMsg, 0, sizeof(SendMsg));
            printf("%s未能从%s登录\n", count, inet_ntoa(addr.sin_addr));
            return;
        }
        
        sprintf(SendMsg, "update userinfo set online = 1 where count = '%s'", count);
        mysql_query(&mysql, SendMsg);
        memset(SendMsg, 0, sizeof(SendMsg));
        sprintf(SendMsg, "update userinfo set socket = %d where count = '%s'", ClientSocket, count);
        mysql_query(&mysql, SendMsg);
        memset(SendMsg, 0, sizeof(SendMsg));
        Close_Database(mysql);
    }
}




