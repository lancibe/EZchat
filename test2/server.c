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


void my_err(const char*, int);
MYSQL Connect_Database(void);



int main(int argc, char** argv)
{
	char SendMsg[1500] = {0} ; 
	char RecvMsg[1500] = {0};

	//创建一个TCP套接字
	int server = socket(AF_INET, SOCK_STREAM, 0);				
	if(server < 0)
		my_err("socket", __LINE__);

    struct sockaddr_in sock;
	sock.sin_family = AF_INET;
	sock.sin_port = htons(14233);
	sock.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定套接字
	if(bind(server, (const struct sockaddr*)&sock, sizeof(struct sockaddr_in)) < 0)
		my_err("bind", __LINE__);

	//将套接字设置为监听模式，待处理队列最大值设置为10
	if(listen(server, 10) < 0)																
		my_err("listen", __LINE__);

	//创建客户端链接
	struct sockaddr_in ClientMsg;
	int len = sizeof(ClientMsg);
	int SocketClient = accept(server,(struct sockaddr*)&ClientMsg, &len);
	
	//欢迎客户端
	printf("%s已经连接成功\n", inet_ntoa(ClientMsg.sin_addr));
	if(send(SocketClient, "你已连接至服务器", strlen("你已连接至服务器"), 0) < 0)
		my_err("send", __LINE__);

	

    return 0;
}


//自定义错误处理函数
void my_err(const char* err_string, int line)
{
	fprintf(stderr, "line:%d  ", line);
	perror(err_string);

	exit(1);
}

// 连接数据库
MYSQL Connect_Database(void)
{
    MYSQL mysql;
    //初始化一个句柄
    if(NULL == mysql_init(&mysql))
    {
		my_err("mysql_init", __LINE__);
	}

	//初始化数据库
	if(mysql_library_init(0, NULL, NULL) != 0)
    {
		my_err("mysql_library_init", __LINE__);
	}

	//连接数据库
	if(NULL == mysql_real_connect(&mysql, "localhost", "root", "Zhangyixun1", "users_test", 0, NULL, 0))
    {
		my_err("mysql_real_connect",__LINE__);
	}


	//设置中文字符集
	if(mysql_set_character_set(&mysql, "utf8") < 0)
    {
		my_err("mysql_set_character_set", __LINE__);
	}
	return mysql;
}