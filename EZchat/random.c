

#include "global.h"
#include "random.h"

//此函数仅仅是为了创建随机数
void Random(char* count)
{
    //创建从数字0到9之间的数字
    char tempres[16];
    int temp;  
    srand((unsigned)time(NULL));
    temp = rand();

    sprintf(tempres, "%d", temp);
    strncpy(count, tempres, 8);
}