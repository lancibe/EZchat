// 基于C/S模型的服务端
// gcc server.c -o server -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl
// i686-w64-mingw32-gcc server.c -o server.exe -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl

#include <mysql/mysql.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/epoll.h>
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
	MYSQL mysql = Connect_Database();

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

/* 	//创建客户端链接
	struct sockaddr_in ClientMsg;
	int len = sizeof(ClientMsg);
	int SocketClient = accept(server,(struct sockaddr*)&ClientMsg, &len);//这里创建的套接字就是对应某个客户端的套接字
	
	//欢迎客户端
	printf("%s已经连接成功\n", inet_ntoa(ClientMsg.sin_addr));
	if(send(SocketClient, "你已连接至服务器", strlen("你已连接至服务器"), 0) < 0)
		my_err("send", __LINE__); */


	//使用epoll实现I/O
	int Sockets, SocketNum;
	struct epoll_event ListenEvent,Events[64];
	Sockets = epoll_create(64);
	
	ListenEvent.events = EPOLLIN;
	ListenEvent.data.fd = server;
	epoll_ctl(Sockets, EPOLL_CTL_ADD, server, &ListenEvent);
	while(1)
	{
		int num,i;
		num = epoll_wait(Sockets, Events, 64, 3000);
		for(i = 0 ; i < num ; i++)
		{
			int tempSocket = Events[i].data.fd;

			if(Events[i].events & EPOLLIN)
			{
				if(tempSocket == server)//说明有新连接
				{
					//创建对应新连接用户的套接字
					struct sockaddr_in ClientMsg;
					int len = sizeof(ClientMsg);
					int SocketClient = accept(server, (struct sockaddr*)&ClientMsg, &len);
					//向客户端发送欢迎信息
					printf("%s已经连接成功\n", inet_ntoa(ClientMsg.sin_addr));
					if(send(SocketClient, "服务器信息：你已连接至服务器！", strlen("服务器信息：你已连接至服务器！"), 0) < 0)
						my_err("send", __LINE__); 


				
					//给即将上树的结构体初始化
					struct epoll_event tempEvent;
					tempEvent.events = EPOLLIN;
					tempEvent.data.fd = SocketClient;

					//上树
					epoll_ctl(Sockets, EPOLL_CTL_ADD, SocketClient, &tempEvent);
				}
				else	//这里是有数据到来
				{
					//接收服务器信息
					int res;
					if((res = recv(tempSocket, RecvMsg, sizeof(RecvMsg)-1 , 0)) < 0)
						my_err("recv", __LINE__);

					if(res == 0)
					{
						epoll_ctl(Sockets, EPOLL_CTL_DEL, tempSocket, NULL);//下树
						close(tempSocket);
					}
					else if (res < 0)
					{
						my_err("read", __LINE__);
					}

					do{
						//将服务器发来的消息打到stdin
						RecvMsg[res] = '\0';
						printf("\033[32m%s\033[0m", RecvMsg);
					}while((res = recv(tempSocket, RecvMsg, sizeof(RecvMsg)-1 , 0)) > 0);
				}	
			}

			else if (Events[i].events & EPOLLOUT)
			{
				//处理写事件
			}

			else if (Events[i].events & EPOLLERR)
			{
				//处理错误
			}
		}
	}
	close(server);
	close(Sockets);
	
		
		





	







	/* 
	  * 清理套接字，防止内存泄露
	  * FD_CLR(server, &ClientSockets);
	  * shutdown(server, SHUT_RDWR);
	  */



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
    if(NULL == mysql_init(&mysql)) {
		my_err("mysql_init", __LINE__);
	}

	//初始化数据库
	if(mysql_library_init(0, NULL, NULL) != 0) {
		my_err("mysql_library_init", __LINE__);
	}

	//连接数据库
	if(NULL == mysql_real_connect(&mysql, "localhost", "root", "Zhangyixun1", "users_test", 0, NULL, 0)) {
		my_err("mysql_real_connect",__LINE__);
	}

	//设置中文字符集
	if(mysql_set_character_set(&mysql, "utf8") < 0) {
		my_err("mysql_set_character_set", __LINE__);
	}
	return mysql;
}