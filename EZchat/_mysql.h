#ifndef __MYSQL_H
#define __MYSQL_H
#include "global.h"



MYSQL Connect_Database(void);
void* FindSameCount(char* count, MYSQL mysql);
void InsertUser(char* nickname, char* count, char* passwd, MYSQL mysql);



#endif // !__MYSQL_H