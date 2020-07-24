#ifndef __REFLECTS_H
#define __REFLECTS_H

#include "global.h"

int Analyse(char* buf, int ClientSocket, MYSQL mysql);
int Reflect(char*buf, int flag1, int flag2, int ClientSocket, MYSQL mysql);
int Signup(int ClientSocket, MYSQL mysql);






#endif // !__REFLECTS_H