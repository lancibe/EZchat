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
    else if (strcmp(reflect, "signout") == 0) {
        Signout(ClientSocket);
    }
    else if (strcmp(reflect, "myfriends") == 0){
        Myfriends(ClientSocket);
    }
    else if (strcmp(reflect, "sendmsg") == 0){
        PrivateChat(ClientSocket);
    }
    else if (strcmp(reflect, "checkfriend") == 0){
        CheckFriend(ClientSocket);
    }
    else if(strcmp(reflect, "findpassword") == 0){
        FindPassword(ClientSocket);
    }
    else if(strcmp(reflect, "changepassword") == 0){
        ChangePassword(ClientSocket);
    }
    else if(strcmp(reflect, "addfriend") == 0 || strcmp(reflect, "delfriend") == 0 || strcmp(reflect, "blockfriend") == 0 || strcmp(reflect, "specialcarefriend") == 0) {
        ChangeFriend(ClientSocket, reflect[0]);
    }
}


int Signup(int ClientSocket)
{
    char SendMsg[1500]="请输入您的昵称:)";
    char RecvMsg[1500];
    char nickname[21], *passwd, count[9];
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
    decrypt(RecvMsg, &passwd, len);


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
	    sprintf(res, "insert into userinfo values(default,'%s',%d,'%s',0,-1,'0')", Nickname, Count, passwd);
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
        char* temp = NULL;
        decrypt(RecvMsg, &temp, res);

        //将从客户端接收来的密码和数据库进行比对
        if(strcmp(temp, passwd) == 0)
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
        
        sprintf(SendMsg, "update userinfo set online = 1, socket = %d, ip= '%s' where count = '%s'", ClientSocket, inet_ntoa(addr.sin_addr), count);
        mysql_query(&mysql, SendMsg);
        memset(SendMsg, 0, sizeof(SendMsg));       
        Close_Database(mysql);

        //用户上线之后，向他发送所有的数据库离线信息
        SendDatabaseMsg(ClientSocket);
    }
}



void Signout(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len, i, j, flag;

    MYSQL mysql;
    if(JudgeOnline(ClientSocket, mysql))
    {
        //在线
        memset(SendMsg, 0, sizeof(SendMsg));
        sprintf(SendMsg, "Bye~  :D");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__);

        mysql = Connect_Database();
        memset(SendMsg, 0, sizeof(SendMsg));
        sprintf(SendMsg, "update userinfo set online = 0, socket = -1, ip= '0' where socket = %d", ClientSocket);
        mysql_query(&mysql, SendMsg);
        memset(SendMsg, 0, sizeof(SendMsg));
        Close_Database(mysql);       
    }
    else
    {
        //不在线
        memset(SendMsg, 0, sizeof(SendMsg));
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__);
        return;
    }
}


void Myfriends(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len, i, j, flag1,flag;

    MYSQL mysql;
    if(JudgeOnline(ClientSocket, mysql))
    {
        //在线
        mysql = Connect_Database();
        sprintf(SendMsg, "select id,count,nickname from userinfo where socket = '%d'", ClientSocket);
        flag = mysql_query(&mysql, SendMsg);
        if(flag)
            my_err("mysql_query", __LINE__);
        memset(SendMsg, 0, sizeof(SendMsg));     
        
        sprintf(SendMsg, "这是你的好友列表:");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__);


        MYSQL_RES           *result = NULL;
        MYSQL_ROW           row;
        MYSQL_FIELD       *field;
        char id[3],acount[9], bcount[9],nickname[21];
        int relationship;
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(id, row[0]); 
            strcpy(acount, row[1]);
            strcpy(nickname, row[2]);
        }

        sprintf(SendMsg, "select * from friendship where `acount` = '%s'", acount);
        flag = mysql_query(&mysql, SendMsg);
        if(flag)
            my_err("mysql_query", __LINE__);
        memset(SendMsg, 0, sizeof(SendMsg));     
        
        result = mysql_store_result(&mysql);
        int num_fields;
        if(result)
        {
            field = mysql_fetch_field(result);
            num_fields = mysql_num_fields(result);

            while((row=mysql_fetch_row(result)) != NULL){	
				//打印数据，for循环不知道几列。直到读取完
				for(i = 0 ; i < num_fields ; i++)
				{
                    strcpy(bcount, row[2]);
                    relationship = atoi(row[3]);

                    //这时候已经得到了 A、B的账号，和A对B的关系
                    //接下来应该在userinfo中查找到B的昵称 然后发送到客户端
                    char temp[256];
                    sprintf(temp, "select nickname from userinfo where `count` = '%s'", bcount);
                    flag1 = mysql_query(&mysql, temp);
                    if(flag)
                        my_err("mysql_query", __LINE__);
                    memset(temp, 0, sizeof(temp));

                    MYSQL_RES           *result1 = NULL;
                    MYSQL_ROW           row1;
                    char bnickname[21];

                    if(result1)
                    {
                        row1 = mysql_fetch_row(result1);
                        strcpy(bnickname, row1[0]);
                        strcpy(bcount, row1[1]);
                        printf("\t\t账号%s:\t昵称%s\n", bcount, bnickname);
                    }
                    sprintf(SendMsg, "\t\t账号%s:\t昵称%s", bcount, bnickname);
                    if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                        my_err("send", __LINE__);
                    memset(SendMsg, 0, sizeof(SendMsg)); 
                }
            }
            
            sprintf(SendMsg, "\t\t没有更多了...");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__);
            memset(SendMsg, 0, sizeof(SendMsg));
        }
        Close_Database(mysql);
    }
    else
    {
        //不在线
        memset(SendMsg, 0, sizeof(SendMsg));
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__);
        return;
    }
    
}



void PrivateChat(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len, i, j, flag1,flag;
    int res;
    char countA[9], countB[9];
    MYSQL mysql;   
    MYSQL_RES           *result = NULL;
    MYSQL_ROW           row;
    if(JudgeOnline(ClientSocket, mysql))
    {
        //确认在线后，先进行接收，接收私聊的用户名
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';
        
        mysql = Connect_Database();
        sprintf(SendMsg, "select count from userinfo where socket = '%d'", ClientSocket);
        if(flag)
            my_err("mysql_query", __LINE__);
        memset(SendMsg, 0, sizeof(SendMsg));     
        
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(countA, row[0]);//储存A账号
        }
        
        //收到用户名后，先确认是否有此名
        sprintf(SendMsg, "select count from userinfo where nickname = '%s'", RecvMsg);
        flag = mysql_query(&mysql, SendMsg);
        if(flag)
            my_err("mysql_query", __LINE__);
        memset(SendMsg, 0, sizeof(SendMsg));     
        
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(countB, row[0]);//储存B账号
        }

        //两个账号都经过确认之后，判断两者好友关系
        int Judge1 = JudgeFriend(ClientSocket, mysql, countA, countB);
        int Judge2 = JudgeFriend(ClientSocket, mysql, countB, countA);
        if((Judge1 == 1 || Judge1 ==2) && (Judge2==1|| Judge2 == 2))
        {
            //两项都确认完后，指示客户端开始输入
            strcpy(SendMsg, "Orz请开始你的表演");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));

            char TransitMsg[1500];
            //最新的想法：不管。你爱离线不离线，我把所有的数据存到数据库里，你20年之后上线我就20年之后在你一上线就全发给宁
            while(1)
            {
                memset(RecvMsg, 0, sizeof(RecvMsg));
                if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                    my_err("recv", __LINE__);
                RecvMsg[res] = '\0';
                
                if(strcmp(RecvMsg, "$close$") == 0)
                    break;
                
                //将用户的信息存入数据库
                sprintf(TransitMsg, "insert into msg values(default, '%s', '%s', NOW(), 0, '%s'", countA, countB, RecvMsg);
                if(0 != mysql_query(&mysql, TransitMsg))
                    my_err("mysql_query", __LINE__);

                memset(TransitMsg, 0, sizeof(TransitMsg));
            }
    
            Close_Database(mysql);
        }
        else{
            strcpy(SendMsg, "\033[31m你必须先加对方为好友，才能发起对话\033[0m\n");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));
            return;
        }


    }
    else
    {
        //不在线，指A不在线，因为A不在线肯定是无法往数据库存放东西的
        memset(SendMsg, 0, sizeof(SendMsg));
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__);
        return;       
    }
    
}


//离线消息发送函数
void SendDatabaseMsg(int ClientSocket)
{
    char SendMsg[1500], RecvMsg[1500];
    int i,j,flag;
    MYSQL mysql = Connect_Database();
    MYSQL_RES           *result = NULL;
    MYSQL_ROW           row;
    MYSQL_FIELD       *field;
    char temp[256];
    char count[9];

    //通过此登陆的账户的套接字获得其账号
    sprintf(temp, "select count from userinfo where socket = %d", ClientSocket);
    flag = mysql_query(&mysql, temp);
    if(flag)
        my_err("mysql_query", __LINE__);
    memset(temp, 0, sizeof(temp));     
    
    result = mysql_store_result(&mysql);
    if(result)
    {
        row = mysql_fetch_row(result);
        strcpy(count, row[0]);
    }

    //获得账号之后，通过账号搜索到所有接收人是该账号的数据条目，并且发送给该套接字
    sprintf(temp, "select * from msg where recvcount='%s' and haveread = 0", count);
    flag = mysql_query(&mysql, temp);
    if(flag)
        my_err("mysql_query", __LINE__);
    memset(temp, 0, sizeof(temp));     
    if(result)
    {
        row = mysql_fetch_row(result);
        for(i = 0 ; i < mysql_num_fields(result) ; i++)
        {
            char sendcount[9],recvcount[9];
            char sendtime[64];
            char nickname[21];
            char SendMsgs[1500];
            strcpy(sendcount, row[1]);
            strcpy(recvcount, row[2]);
            strncpy(sendtime, &row[3][6], sizeof(char) * 14);
            sendtime[14] = '\0';
            strcpy(SendMsgs, row[5]);

            MYSQL_RES           *result1 = NULL;
            MYSQL_ROW           row1;
            char TEmp[256];
            sprintf(TEmp, "select nickname from userinfo where count = '%s'", sendcount);
            flag = mysql_query(&mysql, temp);
            if(flag)
                my_err("mysql_query", __LINE__);
            memset(TEmp, 0, sizeof(TEmp));
            if(result1)
            {
                row1 = mysql_fetch_row(result1);
                strcpy(nickname, row1[0]);
            }

            //经过一系列垃圾程序，最终还是将数据库中储存的信息一条一条地发到客户端
            sprintf(SendMsg, "[\033[35m%s\033[0m]\033[32m%s\033[0m对\033[32m你\033[0m说:\n\t%s", sendtime, nickname, SendMsgs);
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));
            //发送完之后，把数据库中的haveread标记为1，表示已读
            sprintf(TEmp, "update msg set `haveread` = '1' where sendtime='%s' and msg='%s'", row[3], row[5]);
        }

        //离线数据发送完之后，给客户端一个信号，让两边都结束程序的运行
        sprintf(SendMsg, "$finished$");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));
    }
}


//用于查看某一个好友的状态
void CheckFriend(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len, i, j, flag1,flag;
    int res;
    MYSQL mysql;   
    MYSQL_RES           *result = NULL;
    MYSQL_ROW           row;
    char acount[9], bcount[9];
    char nickname[21];
    char temp[256];
    int online;
    
    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要查找的好友昵称:");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));   

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';
        strcpy(nickname, RecvMsg);

        //从套接字获取acount
        sprintf(temp, "select count from userinfo where socket = '%d'", ClientSocket);
        flag = mysql_query(&mysql, temp);
        memset(temp, 0, sizeof(temp));
        if(flag) {
                my_err("mysql_query", __LINE__);
        }
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(acount, row[0]);
        }



        //从昵称获取b的在线状态和bcount
        sprintf(temp, "select online, count from userinfo where nickname =  '%s'", nickname);
        flag = mysql_query(&mysql, temp);
        memset(temp, 0, sizeof(temp));
        if(flag) {
                my_err("mysql_query", __LINE__);
        }

        if(result)
        {
            row = mysql_fetch_row(result);
            online = atoi(row[0]);
            strcpy(bcount, row[1]);
        }
        else
        {
            sprintf(SendMsg, "你没有此好友或查无此人");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));    
            return;
        }

        //现在既有acount 又有bcount
        flag1 = JudgeFriend(ClientSocket, mysql, acount, bcount);
        switch (flag1)
        {
            case 0:
                sprintf(SendMsg, "你没有此好友或查无此人");
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));    
                break;
            
            case 1:
                if(online == 1)
                    sprintf(SendMsg, "你和%s是好友，他在线", nickname);
                else
                    sprintf(SendMsg, "你和%s是好友，他不在线", nickname);
                
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));    
                break;

            case 2:
                if(online == 1)
                    sprintf(SendMsg, "%s是你的特别关心好友，他在线", nickname);
                else
                    sprintf(SendMsg, "%s是你的特别关心好友，他不在线", nickname);
                
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg)); 
                break;

            case 3:
                if(online == 1)
                    sprintf(SendMsg, "%s被你拉黑了，他在线", nickname);
                else
                    sprintf(SendMsg, "%s被你拉黑了，他不在线", nickname);
                
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg)); 
                break;


            default:
                break;
        }
    }
    else
    {
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));    
    }

    Close_Database(mysql);
}


void FindPassword(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len, i, j, flag1,flag;
    int res;
    MYSQL mysql = Connect_Database();   
    MYSQL_RES           *result = NULL;
    MYSQL_ROW           row;
    char temp[256];
    int id;
    char *passwd, count[9];

    if(JudgeOnline(ClientSocket, mysql))
    {
        //先检测用户是不是以root身份登录的
        sprintf(temp, "select id from userinfo where socket = '%d'", ClientSocket);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        if(result)
        {
            row = mysql_fetch_row(result);
            id = atoi(row[0]);
        }
        if(id != 1)
        {
            sprintf(SendMsg, "请以root身份登录");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));    
            return;
        }
        else
        {
            //这里，用户既登录账号，又是以root身份登录的开始，修改密码
            sprintf(SendMsg, "请输入需更改的账号");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));

            memset(RecvMsg, 0, sizeof(RecvMsg));
            if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                my_err("recv", __LINE__);
            RecvMsg[res] = '\0';

            //检测账号是否存在
            for(i = 0 ; i < res ; i++)
            {
                if(RecvMsg[i] == '0')
                    continue;
                else 
                    break;
            }
            sprintf(temp, "select count,passwd from userinfo where count = %s", &RecvMsg[i]);

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
                    strcpy(count, row[0]);
                    strcpy(passwd, row[1]);
                }
                sprintf(SendMsg, "请输入密码");
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));    

                memset(RecvMsg, 0, sizeof(RecvMsg));
                if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                    my_err("recv", __LINE__);
                RecvMsg[res] = '\0';

                decrypt(RecvMsg, &passwd, res);
                sprintf(RecvMsg, "UPDATE `EZchat`.`userinfo` SET `passwd` = '%s' WHERE count = '%s'", passwd, count);
                if(mysql_query(&mysql, RecvMsg))
                    my_err("mysql_query", __LINE__);
            }
        }
    }
    else
    {
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));    
    }
}



void ChangePassword(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len, i, j, flag1,flag;
    int res;
    MYSQL mysql = Connect_Database();   
    MYSQL_RES           *result = NULL;
    MYSQL_ROW           row;
    char temp[256];
    char* passwd;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "输入新密码");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';
        decrypt(RecvMsg, &passwd, res);
        sprintf(RecvMsg, "UPDATE `EZchat`.`userinfo` SET `passwd` = '%s' WHERE socket = '%d'", passwd, ClientSocket);
        mysql_query(&mysql, RecvMsg);
    }
    else
    {
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));    
    }
}



void ChangeFriend(int ClientSocket, char kind)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len, i, j, flag1,flag;
    int res;
    char temp[256];
    char acount[9], bcount[9];
    MYSQL mysql = Connect_Database();   
    MYSQL_RES           *result = NULL;
    MYSQL_ROW          row;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要进行操作的账号:");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg)); 
        
        sprintf(temp, "select count from userinfo where socket = '%d'", ClientSocket);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(acount, row[0]);
        }
        memset(temp, 0, sizeof(temp));

        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0';  
        //接收到要进行操作的账号后，先进行检查看有无此账号
        int order;
        for(i = 0 ; i < res ; i++)
        {
            if(RecvMsg[i] == '0')
                continue;
            else 
                break;
        }
        sprintf(temp, "select * from userinfo where count = %s", &RecvMsg[i]);
        if(mysql_query(&mysql, temp))
            my_err("mysql_query", __LINE__);
        strcpy(bcount, &RecvMsg[i]);
        
        memset(temp, 0, sizeof(temp));
        result = mysql_store_result(&mysql);
        if(result)
        {
            switch (kind)
            {
                case 'a':
                    order = 1;
                    sprintf(temp, "INSERT INTO `EZchat`.`friendship` (`id`, `acount`, `bcount`, `relationship`) VALUES (default, '%s', '%s', '%d')", acount, bcount, order);
                    break;
                
                case 'd':
                    order = 0;
                    sprintf(temp, "DELETE FROM `EZchat`.`friendship` WHERE `acount` = '%s' and `bcount` = '%s'", acount, bcount);
                    break;
                
                case 'b':
                    order = 3;
                    sprintf(temp, "UPDATE `EZchat`.`friendship` SET `relationship` = '%d' WHERE `acount` = '%s' and `bcount` = '%s'", order, acount, bcount);
                    break;

                case 's':
                    order  = 2;
                    sprintf(temp, "UPDATE `EZchat`.`friendship` SET `relationship` = '%d' WHERE `acount` = '%s' and `bcount` = '%s'", order, acount, bcount);
                    break;
                
                default:
                    break;
            }
            mysql_query(&mysql, temp);
            memset(temp, 0, sizeof(temp));
        }
        else
        {
            sprintf(SendMsg, "账号不存在");
            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                my_err("send", __LINE__); 
            memset(SendMsg, 0, sizeof(SendMsg));   
        }
            

    }
    else
    {
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));   
    }
}