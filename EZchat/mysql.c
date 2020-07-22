#include "global.h"
#include "_mysql.h"
#include "myerr.h"



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

