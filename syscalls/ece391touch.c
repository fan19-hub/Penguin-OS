#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"
int main()
{
    int32_t fd,cnt;
    uint8_t buf[1024];

    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
	return 3;
    }
    if(-1==(fd=ece391_open(buf))){
        ece391_fdputs (1, (uint8_t*)"could not open the file\n");
        return 2;
    }
    if(-1==(cnt=ece391_write(fd,(uint8_t*)"\0",0))){
        ece391_fdputs (1, (uint8_t*)"could not write the file\n");
        return 2;
    }
    if(ece391_close(fd)==-1)
    {
        ece391_fdputs (1, (uint8_t*)"could not close the file\n");
        return 3;
    }
    return 0;
}