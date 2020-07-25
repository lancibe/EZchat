#ifndef __ORDERS_H
#define __ORDERS_H

#ifndef __CLIENT_C
#define __CLIENT_C
#endif // !__CLIENT_C
#include "global.h"

int AnalyseOrder(char* buf, int Socket);
int JudgeOrder(char*buf, int flag1, int flag2, int Socket);
void Help(void);
void SignupC(int Socket);
void SigninC(int Socket);
void SignoutC(int Socket);
void Exit(int Socket);


#endif // !__ORDERS_H