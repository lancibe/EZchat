#ifndef __LOG_H
#define __LOG_H

#define LEVEL_INFO                                  (0x00)
#define LEVEL_WARNING                        (0x01)
#define LEVEL_ERROR                              (0x02)
#define LEVEL_FATAL                                (0x03)
#define LEVEL_INDISPENSABLE           (0x04)

#define LOG_BUF_LENTH                       256

void SetLogFileName(char* filename);
int LogTime(void);
int LogInit(void);
int LogString(int level, const char* str);
int LogClose(void);




#endif