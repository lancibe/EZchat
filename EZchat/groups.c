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