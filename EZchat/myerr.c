#include "global.h"
#include "myerr.h"
#include "log.h"

//自定义错误处理函数
void my_err(const char* err_string, int line)
{
	fprintf(stderr, "line:%d  ", line);
	perror(err_string);
	LogString(LEVEL_ERROR, err_string);
	exit(1);
}
