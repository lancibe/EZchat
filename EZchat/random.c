

#include "global.h"
#include "random.h"

//此函数仅仅是为了创建随机数
char Random(void)
{
    //创建从数字0到9之间的数字
    srand((unsigned)time(NULL));
    return (rand()%(57-48+1) +48);
}