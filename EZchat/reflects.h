#ifndef __REFLECTS_H
#define __REFLECTS_H

#include "global.h"

int Analyse(char* buf, int ClientSocket);
int Reflect(char*buf, int flag1, int flag2, int ClientSocket);
int Signup(int ClientSocket);
void Signin(int ClientSocket);






#endif // !__REFLECTS_H