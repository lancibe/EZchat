/* 
  * 这可以让其他文件更加美观，不会被头文件占去大量空间
  */


#ifndef __GLOBAL_H
#define __GLOBAL_H


#include <sys/types.h>
#include <sys/socket.h>
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
#include <poll.h>
#include <malloc.h>
#include <pthread.h>
#include <fcntl.h>


//这一部分是为了降低编译难度，客户端不需要的
# ifndef __CLIENT_C
#include <mysql/mysql.h>
#include "_mysql.h"
#include "reflects.h"
#endif


//这一部分是客户端独有的
#ifdef __CLIENT_C
//客户端创建的服务器套接字
static int server;
static char Msg[1500] = {0};
static int signal; 
pthread_mutex_t mutex;
#include "orders.h"
#endif // __CLIENT_C

#include "myerr.h"
#include "log.h"
#include "random.h"





#endif