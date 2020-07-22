// 基于C/S模型的服务端
// gcc server.c mysql.c log.c myerr.c -o server -lpthread -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl
// i686-w64-mingw32-gcc server.c -o server.exe -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl

#include "global.h"



int main(int argc, char** argv)
{
	LogInit();
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
	
	ListenEvent.events = EPOLLIN|EPOLLET;
	ListenEvent.data.fd = server;
	epoll_ctl(Sockets, EPOLL_CTL_ADD, server, &ListenEvent);
	while(1)
	{
		int num,i, flag;
		num = epoll_wait(Sockets, Events, 64, 3000);
		struct sockaddr_in ClientMsg;
		flag  = fcntl(Sockets, F_GETFL);
		flag |= O_NONBLOCK;
		fcntl(Sockets, F_SETFL, flag); 

		for(i = 0 ; i < num ; i++)
		{
			int tempSocket = Events[i].data.fd;
			
			if(Events[i].events & EPOLLIN)
			{
				if(tempSocket == server)//说明有新连接
				{
					//创建对应新连接用户的套接字
					
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
						printf("%s已经离线\n", inet_ntoa(ClientMsg.sin_addr));
						close(tempSocket);
					}
					else if (res < 0)
					{
						my_err("read", __LINE__);
					}

					do{
						//将服务器发来的消息打到stdin
						RecvMsg[res] = '\0';
						printf("\033[34m%s\033[0m", RecvMsg);
					}while((res = recv(tempSocket, RecvMsg, sizeof(RecvMsg)-1 , 0)) > 0);
				}	
			}

			else if (Events[i].events & EPOLLOUT)
			{
				//处理写事件
			}

			else if (Events[i].events & EPOLLERR)
			{

			}
		}
	}
	close(server);
	close(Sockets);
	
		
		





	








	LogClose();
    return 0;
}

