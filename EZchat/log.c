/*
  * log.c和log.h这两个文件是为了记录服务器运行日志
  */

#include "log.h"
#include "global.h"

static FILE* pFile = NULL;
static pthread_mutex_t LogMutex = PTHREAD_MUTEX_INITIALIZER;
static int LogLevel = LEVEL_INDISPENSABLE;
static char LastLogstr[LOG_BUF_LENTH];
static char CurrentTime[128];
static char LogFileName[128] = {"log.txt"};


//仅仅为了写文件名
void SetLogFileName(char* filename)
{
    sprintf(LogFileName, "%s", filename);
}

//记录时间
int LogTime(void)
{
    struct tm* time_stamp;
    time_t Time;

    Time = time(NULL);
    time_stamp = localtime(&Time);

    snprintf(CurrentTime, 128, "%02d/%02d/%02d %02d:%02d:%02d", 
                            time_stamp->tm_mday, time_stamp->tm_mon + 1 , 
                            time_stamp -> tm_year % 100, time_stamp->tm_hour, 
                            time_stamp->tm_min, time_stamp->tm_sec);
    
    return 0;
}

//打开文件输入开头
int LogInit(void)
{
    pFile = fopen(LogFileName, "a");
    if(pFile == NULL){
        my_err("fopen", __LINE__);
    }

    memset(LastLogstr, 0, LOG_BUF_LENTH);

    LogString(LEVEL_INDISPENSABLE, "LOG START\n");
    return 0;
}

//整理输入的字符串
int LogString(int level, const char* str)
{
    char logstr[LOG_BUF_LENTH];
    static int count = 0;
    int needstr = 0;

    pthread_mutex_lock(&LogMutex);
    LogTime();

    if(++count >= 500000)
    {
        count = 0;
        fclose(pFile);
        pFile = fopen(LogFileName, "a");
    }

    if(strcmp(str, LastLogstr))
    {
        switch(level)
        {
            case LEVEL_INFO:
                if(LogLevel <= LEVEL_INFO)
                {
                    needstr = 1;
                    snprintf(logstr, LOG_BUF_LENTH, "[INFO:%s] %s", CurrentTime, str);
                }
                break;

            case LEVEL_WARNING:
                if(LogLevel <= LEVEL_WARNING)
                {
                    needstr = 1;
                    snprintf(logstr, LOG_BUF_LENTH, "[WARNING:%s] %s", CurrentTime, str);
                }
                break;

            case LEVEL_ERROR:
                if(LogLevel <= LEVEL_ERROR)
                {
                    needstr = 1;
                    snprintf(logstr, LOG_BUF_LENTH, "[ERROR:%s] %s", CurrentTime,str);
                }
                break;
            
            case LEVEL_FATAL:
                if(LogLevel <= LEVEL_FATAL)
                {
                    needstr = 1;
                    snprintf(logstr, LOG_BUF_LENTH, "[FATAL:%s] %s", CurrentTime, str);
                }
                break;

            case LEVEL_INDISPENSABLE:
                needstr = 1;
                snprintf(LastLogstr, LOG_BUF_LENTH, "[%s] %s", CurrentTime,str);
                break;
        }

        if(needstr)
        {
            fprintf(pFile, "%s", logstr);
            memset(LastLogstr, 0, LOG_BUF_LENTH);
            strcpy(LastLogstr, str);
        }
    }

    fflush(pFile);
    pthread_mutex_unlock(&LogMutex);
    return 0;
}


int LogClose(void)
{
    if(pFile != NULL)
    {
        LogString(LEVEL_INDISPENSABLE, "\nLOG END\t\t\t:D\n");
        fclose(pFile);
    }
    return 0;
}