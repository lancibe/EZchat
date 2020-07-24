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
