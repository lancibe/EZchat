#include "global.h"
#include "_mysql.h"
#include "myerr.h"
#include <mysql/mysql.h>


// 连接数据库
MYSQL Connect_Database(void)
{
    MYSQL mysql;
    //初始化一个句柄
    if(NULL == mysql_init(&mysql)) {
		my_err("mysql_init", __LINE__);
	}

	//初始化数据库
	if(mysql_library_init(0, NULL, NULL) != 0) {
		my_err("mysql_library_init", __LINE__);
	}

	//连接数据库
	if(NULL == mysql_real_connect(&mysql, "localhost", "root", "Zhangyixun1", "EZchat", 0, NULL, 0)) {
		my_err("mysql_real_connect",__LINE__);
	}

	//设置中文字符集
	if(mysql_set_character_set(&mysql, "utf8") < 0) {
		my_err("mysql_set_character_set", __LINE__);
	}
	return mysql;
}



//查找是否有相同的账号，返回0则没有相同的账号
//功能需完善
void* FindSameCount(char* count, MYSQL mysql)
{
	int flag;
	char order[256];
	MYSQL_ROW row;
	MYSQL_RES *res;
    memset(order, 0, sizeof(order));
	sprintf(order, "select * from userinfo where count = %s", count);
    flag=mysql_real_query(&mysql, order, (unsigned int)strlen(order));
	if(!flag) {
		printf("未查询到该数据\n");
		return NULL;
	}
	else {
		printf("成功查询到该数据\n");
		res = mysql_store_result(&mysql);
		while(row = mysql_fetch_row(res))
		{
			int i;
			for(i = 0 ; i < mysql_num_fields(res); i++)
			{
				printf("%s\t", row[i]);
			}
			printf("\n");
		}
		return (int*)0x10;
	}

}



//关闭数据库
void Close_Database(MYSQL mysql)
{
	mysql_close(&mysql);
	mysql_library_end();
}




//此函数是用于检测套接字参数所表示的ip地址是否在线，在后面的函数中都会用到
int JudgeOnline(int ClientSocket, MYSQL mysql)
{
    mysql = Connect_Database();
	
	struct sockaddr_in addr;
    int length = sizeof(addr);
    getpeername(ClientSocket, (struct sockaddr*)&addr, &length);

	char temp[256];
	sprintf(temp, "select online from userinfo where ip = '%s'", inet_ntoa(addr.sin_addr));
	int flag = mysql_query(&mysql, temp);
	if(flag)
		my_err("mysql_query", __LINE__);
	else
	{
		MYSQL_RES           *result = NULL;
        MYSQL_ROW           row;
        result = mysql_store_result(&mysql);
        if(result)
        {
            row = mysql_fetch_row(result);
			char *online;
			online = row[0];
			Close_Database(mysql);
			return atoi(online);
        }
	}
}
