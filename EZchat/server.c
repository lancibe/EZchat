// 基于C/S模型的服务端
// gcc server.c mysql.c log.c myerr.c -o server -lpthread -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl
// i686-w64-mingw32-gcc server.c -o server.exe -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -ldl

#include <mysql/mysql.h>
#include "global.h"
//线程池的线程数量
#define THREAD_MAX 64

//监听端口
#define LISTEN_PORT 14233

//线程间通信变量
char Msg[64][1500];             //每个线程可以通过使用其内部的pool_index来获取相应的变量

/* 
  * 无用
  * 监听端口的数量,从LISTEN_PORT到LISTEN_PORT+LISTEN_MAX-1 
  */
#define LISTEN_MAX 1 

#define SERVER_IP "192.168.31.136" 

typedef struct
{ 
    char ip4[128]; 
    int     port; 
    int     fd; 
} listen_info; 

//服务器参数 
static listen_info s_listens[LISTEN_MAX]; 

//线程池参数 
static unsigned int s_thread_para[THREAD_MAX][8];//线程参数 
static pthread_t s_tid[THREAD_MAX];//线程ID 
pthread_mutex_t s_mutex[THREAD_MAX];//线程锁 

//创建线程池
static int init_thread_pool(void); 
//创建监听套接字
static int init_listen4(char *ip4, int port, int max_link); 

//线程函数，创建成了线程对，每两个线程负责对一个客户端进行收发
void* execute(unsigned int thread_para[]); 



int main(int argc, char *argv[])
{ 
    int i, j, rc; 

    int sock_listen; //监听套接字 
    int sock_cli; //客户端连接 
    int listen_index; 

    int epfd; 
    int nfds; 
    struct epoll_event ev; 
    struct epoll_event events[LISTEN_MAX]; 

    socklen_t addrlen; //地址信息长度 
    struct sockaddr_in addr4; //IPv4地址结构 

    //线程池初始化 
    if((rc = init_thread_pool()) != 0)
        my_err("init_thread_pool", __LINE__) ;


    //初始化服务监听 
    for (i = 0; i < LISTEN_MAX; i++) 
    { 
        sprintf(s_listens[i].ip4, "%s", SERVER_IP); 
        s_listens[i].port = LISTEN_PORT + i; 
        //创建监听 
        rc = init_listen4(s_listens[i].ip4, s_listens[i].port, 16); 
        if (0 > rc) 
        { 
            fprintf(stderr, "无法创建监听套接字于%s:%d:(\n", s_listens[i].ip4, s_listens[i].port); 
            my_err("listen", __LINE__); 
        } 
        s_listens[i].fd = rc; 
    } 

    //使用epoll族来实现I/O多路复合模型
    epfd = epoll_create(32); 
    for (i = 0; i < LISTEN_MAX; i++) 
    { 
        //加入epoll事件集合 
        ev.events = EPOLLIN|EPOLLOUT; 
        ev.data.u32 = i;//记录listen数组下标 
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, s_listens[i].fd, &ev) < 0)  { 
            my_err("epoll_ctl", __LINE__);
        } 
    } 

    //服务循环 
    while(1)
    { 
        //等待epoll事件 
        nfds = epoll_wait(epfd, events, LISTEN_MAX, -1); 
        //处理epoll事件 
        for (i = 0; i < nfds; i++) 
        { 
            //接收客户端连接 
            listen_index = events[i].data.u32; 
            sock_listen = s_listens[listen_index].fd; 
            addrlen = sizeof(struct sockaddr_in); 
            memset(&addr4, 0, addrlen);
            sock_cli = accept(sock_listen, (struct sockaddr *)&addr4, &addrlen); 
            if (sock_cli < 0)  { 
                my_err("accept", __LINE__); 
                continue; //注意这里是continue，因为客户端连接失败是客户端的问题，和服务器无关，不需要exit
            } 
            //查询空闲线程对 
            for (j = 0; j < THREAD_MAX; j++) 
            { 
                if (0 == s_thread_para[j][0]) break; 
            } 
            if (j >= THREAD_MAX) 
            { 
                fprintf(stderr, "线程池已满, 连接无效:(\r\n"); 
                shutdown(sock_cli, SHUT_RDWR); 
                close(sock_cli); 
                continue; 
            } 
            //复制有关参数 
            s_thread_para[j][0] = 1;//设置活动标志为"活动" ，这个0/1就是用来当标志的，反映此线程是否在被使用
            s_thread_para[j][1] = sock_cli;//客户端连接 
            s_thread_para[j][2] = listen_index;//服务索引 
            printf("%s已连接\n", s_listens[i].ip4);
            

            //线程解锁 
            pthread_mutex_unlock(s_mutex + j); 
        }
    } 
    
    exit(0); 
} 

static int init_thread_pool(void) 
{ 
    int i, rc; 

    //初始化线程池参数 
    for (i = 0; i < THREAD_MAX; i++) 
    { 
        s_thread_para[i][0] = 0;//设置线程占用标志为"空闲" 
        s_thread_para[i][7] = i;//线程池索引 
        pthread_mutex_lock(s_mutex + i);//线程锁 
    } 

    //创建线程池 
    for (i = 0; i < THREAD_MAX; i++) 
    { 
        rc = pthread_create(s_tid + i, 0, (void *)execute, (void *)(s_thread_para[i])); 
        if (0 != rc) { 
            my_err("pthread_create", __LINE__);
        } 
    } 

    //成功返回 
    return(0); 
} 

static int init_listen4(char *ip4, int port, int max_link) 
{ 
    //临时变量 
    int sock_listen4; 
    struct sockaddr_in addr4; 
    unsigned int optval; 
    struct linger optval1; 

    //初始化数据结构 
    memset(&addr4,0,  sizeof(addr4)); 
    inet_pton(AF_INET, ip4, &(addr4.sin_addr)); 
    addr4.sin_family = AF_INET; 
    addr4.sin_port = htons(port); 

    //创建SOCKET 
    sock_listen4 = socket(AF_INET, SOCK_STREAM, 0); 
    if (0 > sock_listen4) return(-1); 

    if (bind(sock_listen4, (struct sockaddr *)&addr4, sizeof(addr4)) < 0) 
    { 
        close(sock_listen4); 
        my_err("bind", __LINE__);
    } 

    if (listen(sock_listen4, max_link) < 0) 
    { 
        close(sock_listen4); 
        my_err("listen", __LINE__);
    } 

    return(sock_listen4); 
} 




void * execute(unsigned int thread_para[]) 
{ 
    int pool_index; //线程池索引 
    int sock_cli; //客户端连接 
    int listen_index; //监听索引 

    char SendMsg[1500], RecvMsg[1500]; //传输缓冲区 
    char *p; 
    int i, j, len; 

    //线程脱离创建者 
    pthread_detach(pthread_self()); 
    pool_index = thread_para[7]; 


/* 
  * 每个线程都要经过初始化，然后进行工作
  * 这里是大循环套着一个小循环，大循环是，每个线程送走一批又一批的客户端
  * 小循环是在处理过程中，要和客户端进行交互。
  */
    while(1)
    {
        pthread_mutex_lock(s_mutex + pool_index);//等待线程解锁 

        //线程变量内容复制 
        sock_cli = thread_para[1];//客户端套接字 
        listen_index = thread_para[2];//监听索引 
        
        
        //问好
        send(sock_cli, "服务器信息：你已成功连接!", sizeof("服务器信息：你已成功连接!"), 0);
        
        //和客户端互动这是交互最重要的部分
        while(1)
        {
            int res;
            if((res = recv(sock_cli, RecvMsg, sizeof(RecvMsg)-1 , 0)) < 0)
                my_err("recv", __LINE__);
            else if (res == 0)
            {
                printf("%s已下线\n", s_listens[pool_index].ip4);
                break;
            }
            RecvMsg[res] = '\0';
            sprintf(Msg[pool_index], "%s", RecvMsg);
            printf("%s", Msg[pool_index]);
        }


        //释放连接 
        shutdown(sock_cli, SHUT_RDWR); 
        close(sock_cli); 

        //线程任务结束 
        thread_para[0] = 0;//设置线程占用标志为"空闲" 
    }

    pthread_exit(NULL); 
}

