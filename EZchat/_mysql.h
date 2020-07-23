#ifndef __MYSQL_H
#define __MYSQL_H
#include "global.h"


MYSQL mysql;
MYSQL Connect_Database(void);
int FindSameCount(char* count);
void InsertUser(char* nickname, char* count, char* passwd);



#endif // !__MYSQL_H