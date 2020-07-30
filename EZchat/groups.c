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


void ExitGroup(int ClientSocket)
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
        sprintf(SendMsg, "请输入要退出的群号");
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
                sprintf(temp, "delete from groupmember where membercount = '%s'", RecvMsg);                flag = mysql_query(&mysql, temp);
                if(flag)
                    my_err("mysql_query", __LINE__);        

                sprintf(SendMsg, "已退出该群");
                if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                    my_err("send", __LINE__); 
                memset(SendMsg, 0, sizeof(SendMsg));   
            }
            else
            {               
                sprintf(SendMsg, "你不在该群");
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


void SetAdmin(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char temp[256];
    char count[9];
    char groupcount[9], membercount[9];

    MYSQL mysql = Connect_Database();
    MYSQL_RES       *result;
    MYSQL_ROW      row;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要操作的群号");
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
        strcpy(groupcount, RecvMsg);

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
            memset(temp, 0, sizeof(temp));
            if(result)
            {
                sprintf(temp, "select grouphost from groupinfo where groupcount = '%s'", RecvMsg);
                flag = mysql_query(&mysql, temp);
                if(flag)
                    my_err("mysql_query", __LINE__);
                result = mysql_store_result(&mysql);
                memset(temp, 0, sizeof(temp));
                if(result)
                {
                    row = mysql_fetch_row(result);
                    if(strcmp(count, row[0]) == 0)
                    {
                        sprintf(SendMsg, "请输入提升为管理员的成员账号");
                        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                            my_err("send", __LINE__); 
                        memset(SendMsg, 0, sizeof(SendMsg));     

                        memset(RecvMsg, 0, sizeof(RecvMsg));
                        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                            my_err("recv", __LINE__);
                        RecvMsg[res] = '\0'; //接收来的是要被升级为管理员的成员账号
                        strcpy(membercount, groupcount);
                        int flag1 = JudgeMember(ClientSocket, mysql, membercount, groupcount);
                        if(flag1 > 0)
                        {
                            sprintf(temp, "update groupmember set status = '2' where groupcount = '%s' and membercount = '%s'", groupcount, membercount);
                            flag = mysql_query(&mysql, temp);
                            if(flag)
                                my_err("mysql_query", __LINE__);
                            sprintf(SendMsg, "修改管理员成功");
                            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                                my_err("send", __LINE__); 
                            memset(SendMsg, 0, sizeof(SendMsg));       
                        }
                        else
                        {
                            sprintf(SendMsg, "该账号不在该群");
                            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                                my_err("send", __LINE__); 
                            memset(SendMsg, 0, sizeof(SendMsg));       
                        }
                    }
                    else
                    {
                        sprintf(SendMsg, "你不是该群群主");
                        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                            my_err("send", __LINE__); 
                        memset(SendMsg, 0, sizeof(SendMsg));       
                    }
                }
            }
            else
            {               
                sprintf(SendMsg, "你不在该群");
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


int JudgeMember(int ClientSocket, MYSQL mysql, char* membercount, char* groupcount)
{
    char temp[256];
    int flag;    
    MYSQL_RES       *result;
    MYSQL_ROW      row;
    char SendMsg[1500];
    
    sprintf(temp, "select status from groupmember where groupcount = '%s' and membercount = '%s'", groupcount, membercount);
    flag = mysql_query(&mysql, temp);
    if(flag)
        my_err("mysql_query", __LINE__);
    result = mysql_store_result(&mysql);
    if(result)
    {
        row = mysql_fetch_row(result);
        return atoi(row[0]);
    }
    else
    {               
        return -1;
    }
}



void KickOff(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char temp[256];
    char count[9];
    char groupcount[9], membercount[9];

    MYSQL mysql = Connect_Database();
    MYSQL_RES       *result;
    MYSQL_ROW      row;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要操作的群号");
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
        strcpy(groupcount, RecvMsg);

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
            memset(temp, 0, sizeof(temp));
            if(result)
            {
                sprintf(temp, "select grouphost from groupinfo where groupcount = '%s'", RecvMsg);
                flag = mysql_query(&mysql, temp);
                if(flag)
                    my_err("mysql_query", __LINE__);
                result = mysql_store_result(&mysql);
                memset(temp, 0, sizeof(temp));
                if(result)
                {
                    row = mysql_fetch_row(result);
                    if(strcmp(count, row[0]) == 0 || JudgeMember(ClientSocket, mysql, membercount, groupcount) == 2)
                    {
                        sprintf(SendMsg, "请输入踢出的成员账号");
                        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                            my_err("send", __LINE__); 
                        memset(SendMsg, 0, sizeof(SendMsg));     

                        memset(RecvMsg, 0, sizeof(RecvMsg));
                        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
                            my_err("recv", __LINE__);
                        RecvMsg[res] = '\0'; //接收来的是要踢出的成员账号
                        strcpy(membercount, groupcount);
                        int flag1 = JudgeMember(ClientSocket, mysql, membercount, groupcount);
                        if(flag1 > 0)
                        {
                            sprintf(temp, "delete from groupmember where groupcount = '%s' and membercount = '%s'", groupcount, membercount);
                            flag = mysql_query(&mysql, temp);
                            if(flag)
                                my_err("mysql_query", __LINE__);
                            sprintf(SendMsg, "踢出成员成功");
                            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                                my_err("send", __LINE__); 
                            memset(SendMsg, 0, sizeof(SendMsg));       
                        }
                        else
                        {
                            sprintf(SendMsg, "该账号不在该群");
                            if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                                my_err("send", __LINE__); 
                            memset(SendMsg, 0, sizeof(SendMsg));       
                        }
                    }
                    else
                    {
                        sprintf(SendMsg, "你不是该群群主");
                        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                            my_err("send", __LINE__); 
                        memset(SendMsg, 0, sizeof(SendMsg));       
                    }
                }
            }
            else
            {               
                sprintf(SendMsg, "你不在该群");
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


void MyGroups(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char temp[256];
    char count[9];
    char groupcount[9], membercount[9];
    int num_fields;

    MYSQL mysql = Connect_Database();
    MYSQL_RES         *result;
    MYSQL_ROW       row;
    MYSQL_FIELD     *field;

    if(JudgeOnline(ClientSocket, mysql))
    {
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
    

        sprintf(temp, "select groupcount from groupmember where membercount = '%s'", count);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        result = mysql_store_result(&mysql);
        if(result)
        {
            field = mysql_fetch_field(result);
            num_fields = mysql_num_fields(result);
            
            while((row=mysql_fetch_row(result)) != NULL)
            {
                for(i = 0 ; i < num_fields ; i++)
                {
                    sprintf(SendMsg, "\t\t你在群%s内\n", row[0]);
                    if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                        my_err("send", __LINE__); 
                    memset(SendMsg, 0, sizeof(SendMsg));   
                }
            }
            sprintf(SendMsg, "No More");
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


void MyGroup(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char temp[256];
    char count[9];
    char groupcount[9], membercount[9];
    int num_fields;

    MYSQL mysql = Connect_Database();
    MYSQL_RES       *result;
    MYSQL_ROW      row;
    MYSQL_FIELD     *field;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要操作的群号");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        int res;
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 

        sprintf(temp, "select membercount from groupmember where groupcount = '%s'", RecvMsg);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        result = mysql_store_result(&mysql);
        if(result)
        {
            field = mysql_fetch_field(result);
            num_fields = mysql_num_fields(result);
            
            while((row=mysql_fetch_row(result)) != NULL)
            {
                for(i = 0 ; i < num_fields ; i++)
                {
                    sprintf(SendMsg, "\t\t%s在群内\n", row[0]);
                    if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                        my_err("send", __LINE__); 
                    memset(SendMsg, 0, sizeof(SendMsg));   
                }
            }
            sprintf(SendMsg, "No More");
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



void GroupChatHistory(int ClientSocket)
{
    char SendMsg[1500];
    char RecvMsg[1500];
    int len,i,j,flag;
    char temp[256];
    char count[9];
    char groupcount[9];
    int num_fields;

    MYSQL mysql = Connect_Database();
    MYSQL_RES       *result;
    MYSQL_ROW      row;
    MYSQL_FIELD     *field;

    if(JudgeOnline(ClientSocket, mysql))
    {
        sprintf(SendMsg, "请输入要操作的群号");
        if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__); 
        memset(SendMsg, 0, sizeof(SendMsg));

        int res;
        memset(RecvMsg, 0, sizeof(RecvMsg));
        if((res = recv(ClientSocket, RecvMsg, sizeof(RecvMsg) - 1, 0)) < 0)
            my_err("recv", __LINE__);
        RecvMsg[res] = '\0'; 
        strcpy(groupcount, RecvMsg);
        
        //获取账号
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

        sprintf(temp, "select * from groupmsg where recvgroup = '%s'", groupcount);
        flag = mysql_query(&mysql, temp);
        if(flag)
            my_err("mysql_query", __LINE__);
        result = mysql_store_result(&mysql);
        if(result)
        {
            int num_fields;
            num_fields = mysql_num_fields(result);
            while((row=mysql_fetch_row(result)) != NULL)
            {
                for(i = 0 ; i < num_fields ; i++)
                {
                    char sendtime[64];
                    char SendMsgs[1024];
                    char sendcount[9];
                    strcpy(sendcount, row[1]);
                    strcpy(sendtime, row[3]);
                    strcpy(SendMsgs, row[4]);

                    sprintf(SendMsg, "[\033[35m%s\033[0m]\033[32m%s\033[0m说:\n\t%s", sendtime, sendcount, SendMsgs);
                    if(send(ClientSocket, SendMsg, strlen(SendMsg), 0) < 0)
                        my_err("send", __LINE__); 
                    memset(SendMsg, 0, sizeof(SendMsg));
                }
            }
            sprintf(SendMsg, "$close$");
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