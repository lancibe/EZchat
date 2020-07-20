// 基于C/S模型的服务端
// gcc server.c -o server -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl

#include <mysql/mysql.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>



//自定义错误处理函数
void my_err(const char* err_string, int line)
{
	fprintf(stderr, "line:%d  ", line);
	perror(err_string);
	exit(1);
}



//调用数据库
void BindMysql(void)
{
    MYSQL mysql;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	MYSQL_RES *result;


    //初始化一个句柄
    if(NULL == mysql_init(&mysql))
    {
		my_err("mysql_init", __LINE__);
	}
	printf("初始化句柄成功");

	//初始化数据库
	if(mysql_library_init(0, NULL, NULL) != 0)
    {
		my_err("mysql_library_init", __LINE__);
	}
	printf("初始化数据库成功");

	//连接数据库
	if(NULL == mysql_real_connect(&mysql, "localhost", "root", "Zhangyixun1", "users_test", 0, NULL, 0))
    {
		my_err("mysql_real_connect", __LINE__);
	}
	printf("连接数据库成功");

	//设置中文字符集
	if(mysql_set_character_set(&mysql, "utf8") < 0)
    {
		my_err("mysql_set_character_set", __LINE__);
	}
	printf("中文字符集设置成功");

	printf("连接数据库成功");
	unsigned int num_fields = mysql_num_fields(result);

	while (row = mysql_fetch_row(result)) 
	{
		for (int i = 0; i < num_fields; i++) 
		{
			printf("%-20s", row[i]);
		}
		printf("\n");
	}
}    




int main(void)
{
    BindMysql();
    return 0;
}