#ifndef __MYSQL_H
#define __MYSQL_H
#include "global.h"



MYSQL Connect_Database(void);
void* FindSameCount(char* count, MYSQL mysql);
void InsertUser(char* nickname, char* count, char* passwd);
void Close_Database(MYSQL mysql);
int JudgeOnline(int ClientSocket, MYSQL mysql);
int JudgeFriend(int ClientSocket, MYSQL mysql, char acount[9], char bcount[9]);


#endif // !__MYSQL_H