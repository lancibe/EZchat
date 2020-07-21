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
    

    int res;
    if((res = recv(server, RecvMsg, sizeof(RecvMsg)-1 , 0)) < 0)
        my_err("recv", __LINE__);
    RecvMsg[res] = '\0';
    printf("%s\n", RecvMsg);




    return 0;
}


//自定义错误处理函数
void my_err(const char* err_string, int line)
{
	fprintf(stderr, "line:%d  ", line);
	perror(err_string);

	exit(1);
}
