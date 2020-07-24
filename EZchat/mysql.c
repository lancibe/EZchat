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
void* FindSameCount(char* count)
{
	int res;
	char order[256];
	sprintf(order, "select * from Userinfo where count = %s", count);
	res=mysql_real_query(&mysql, order, strlen(order));
	if(res)
		return NULL;
	else
	{
		printf("成功查询到该数据");
		return 0;
	}

}

//将用户数据插入数据库
void InsertUser(char* nickname, char* count, char* passwd)
{
	char res[256];
	sprintf(res, "insert into Userinfo values(default, '%s', %s,'%s')", nickname, count, passwd);
	if(!mysql_query(&mysql, res))
		my_err("mysql_query", __LINE__);
}