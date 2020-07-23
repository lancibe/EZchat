//这是指令判断的部分，应与客户端一起进行编译
#ifndef __CLIENT_C
#define __CLIENT_C
#endif // !__CLIENT_C
#include "global.h"


//用于分析指令
int AnalyseOrder(char* buf)
{
    int flag1,flag2,i,j;
    for(i = 0 ; i < 1500 ; i++)
    {
        if(buf[i] == '$')
        {
            flag1 = i;
            for(j = i+1 ; j < 1500; j++)
            {
                if(buf[j] == '$')
                {
                    flag2 = j;
                    break;
                }
            }


            if(buf[j] != '$')
            {
                //有前无后，指令无效
                printf("\033[31m无效的指令，请重新输入:(\033[0m\n");
                fflush(stdin);
                break;
            }
            else
            {
                //找到前后$了，开始执行命令
                int order = JudgeOrder(buf, flag1, flag2);
                fflush(stdin);
                if(order == 0)
                {
                    printf("\033[31m无效的指令，请重新输入:(\033[0m\n");
                    fflush(stdin);
                    break;
                }
            }
            break;
        }
        else
        {
            //没找到前$，指令无效
            printf("\033[31m请输入正确的指令:(\033[0m\n");
            break;
        }
        fflush(stdin);
    }
}

//用于判断指令
int JudgeOrder(char*buf, int flag1, int flag2)
{
    int i,j;
    char order[1500];
    //先判断第三种非法输入情况
    if((buf[flag1] == '$') && (buf[flag1+1] == '$'))
    {
        fprintf(stderr, "字符串buf错误，连续输入两个$");
        memset(order, 0, 1500);
        return 0;
    }    

    //将命令复制进来
    strncpy(order, &buf[flag1+1], flag2-flag1-1);

    if(strcmp(order, "help") == 0){
        Help();
    }
    else if (strcmp(order, "logup") == 0 || strcmp(order, "signup")==0) {
        
    }
    else {
        fprintf(stderr, "无匹配命令");
        memset(order, 0, 1500);
        return 0;
    }
    

    memset(order, 0, 1500);
    return 1;
}


void Help(void)
{
    FILE * pFile;
    char ch[256];
    pFile = fopen("help.txt", "r");
    while(!feof(pFile))
    {
        fgets(ch, 256, pFile);
        printf("\t\t\t%s\n", ch);
    }

}