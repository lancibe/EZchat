// 基于C/S模型的客户端
// gcc client.c -o client -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl

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



int main(int argc, char** argv)
{
    char SendMsg[1500] = {0};
    char RecvMsg[1500] = {0};

    //创建一个TCP套接字
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0)
        my_err("socket", __LINE__);

    //连接服务器
    struct sockaddr_in ServerMsg;
    ServerMsg.sin_family = AF_INET;
    ServerMsg.sin_port = htons(14233);
    ServerMsg.sin_addr.s_addr = inet_addr("192.168.31.136");//注意这里的端口号和IP地址一定都是服务器的
    if(connect(server, (struct sockaddr*)&ServerMsg, sizeof(struct sockaddr_in)) < 0)
        my_err("connect", __LINE__);
    

    //从服务器接收欢迎信息
    int res;
    if((res = recv(server, RecvMsg, sizeof(RecvMsg)-1 , 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    printf("\033[32m%s\033[0m\n", RecvMsg);

    while(1)
    {
        //向服务器回复信息
        char buf[1500];
        memset(buf, 0, sizeof(buf));
        scanf("%s", buf);
        sprintf(SendMsg, "\033[34m%s\033[1m\n", buf);
        if(send(server, SendMsg, strlen(SendMsg), 0) < 0)
            my_err("send", __LINE__);
    }



    return 0;
}


//自定义错误处理函数
void my_err(const char* err_string, int line)
{
	fprintf(stderr, "line:%d  ", line);
	perror(err_string);

	exit(1);
}
