#include "global.h"
#include "reflects.h"

//这一部分仍是基于reflects.c，对客户端发出的指令进行回应和操作
//groups.c针对于群的操作


void CreateGroup(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char groupname[21];
    char groupcount[9], grouphost[9];
    char temp[256];

    MYSQL mysql = Connect_Database();
    MYSQL_RES       *result;
    MYSQL_ROW      row;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入群名称");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));
        

        int res;
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        strcpy(groupname, RecvMsg);

        Random(groupcount);
        groupcount[8] = '\0';
        strcpy(SendMsg, groupcount);
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));


        sprintf(temp, "select count from userinfo where socket = '%d'", ClientSocket);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(grouphost, row[0]);
        }
        memset(temp, 0, sizeof(temp));

        sprintf(temp, "INSERT INTO `EZchat`.`groupinfo` (`id`, `groupname`, `groupcount`, `grouphost`) VALUES (default, '%s', '%s', '%s')", groupname, groupcount, grouphost);
        if(mysql_query(&mysql, temp))
            my_err("mysql_query", __LINE__);
        
    }
    else
    {
        sprintf(SendMsg, "请先登录");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));   
    }
}


void DelGroup(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char temp[256];
    char count[9];

    MYSQL mysql = Connect_Database();
    MYSQL_RES       *result;
    MYSQL_ROW      row;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要解散的群账号");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));   

        sprintf(temp, "select count from userinfo where socket = '%d'", ClientSocket);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(count, row[0]);
        }
        memset(temp, 0, sizeof(temp));

        int res;
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        if(strcmp(RecvMsg, "close"))
            return;
        else
        {
            sprintf(temp, "select * from groupinfo where grouphost = '%s' and groupcount = '%s'", count, RecvMsg);
            flag = mysql_query(&mysql, temp);
            if(flag)
                my_err("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            memset(temp, 0, sizeof(temp));
            if(result)
            {
                sprintf(temp, "delete from groupinfo where grouphost = '%s' and groupcount = '%s'", count, RecvMsg);
                mysql_query(&mysql, temp);
                memset(temp, 0, sizeof(temp));
                sprintf(temp, "delete from groupmember where groupcount = '%s'", RecvMsg);
                sprintf(SendMsg, "该群已被删除");
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));   
            }
            else
            {
                sprintf(SendMsg, "你不是群主或该群不存在");
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));   
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



void JoinGroup(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char temp[256];
    char count[9];

    MYSQL mysql = Connect_Database();
    MYSQL_RES       *result;
    MYSQL_ROW      row;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要加入的群号");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));   


        sprintf(temp, "select count from userinfo where socket = '%d'", ClientSocket);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
            strcpy(count, row[0]);
        }
        memset(temp, 0, sizeof(temp));


        int res;
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; //接收来的是群号

        sprintf(temp, "select * from groupinfo where groupcount = '%s'",  RecvMsg);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        result = mysql_store_result(&mysql);
        memset(temp, 0, sizeof(temp));
        if(result)
        {
            sprintf(temp, "select status from groupmember where groupcount = '%s' and membercount = '%s'", RecvMsg, count);
            flag = mysql_query(&mysql, temp);
            if(flag)
                my_err("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            if(result)
            {
                row = mysql_fetch_row(result);
                sprintf(SendMsg, "你已在群中");
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));   
            }
            else
            {
                sprintf(temp, "INSERT INTO `EZchat`.`groupmember` (`id`, `groupcount`, `membercount`, `status`) VALUES (default, '%s', '%s', '5')", RecvMsg, count);
                flag = mysql_query(&mysql, temp);
                if(flag)
                    my_err("mysql_query", __LINE__);        

                sprintf(SendMsg, "已申请加群");
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));       
            }
        }
        else
        {
            sprintf(SendMsg, "该群号不存在");
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

    Close_Database(mysql);
}