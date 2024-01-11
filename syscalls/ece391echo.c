#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"
int main(){
    int32_t fd, cnt;
    uint8_t buf[100];
    uint8_t buf2[32];
    if (0 != ece391_getargs (buf, 100)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
	return 3;
    }
    ece391_fdputs (1, (uint8_t*)"write the filename of the file you want to write in\n");
    if (-1 == (cnt = ece391_read (0, buf2, 32))) {
        buf2[cnt-1]='\0';
        ece391_fdputs (1, (uint8_t*)"Can't read input \n");
		return 2;
    }
    if (-1 == (fd = ece391_open (buf2))) {
        ece391_fdputs (1, (uint8_t*)"file not found\n");
	return 2;
    }
    if(-1==(cnt=ece391_write(fd,buf,ece391_strlen(buf)))){
        ece391_fdputs (1, (uint8_t*)"can't write \n");
        return 2;
    }
    if(-1==(cnt=ece391_close(fd))){
        ece391_fdputs (1, (uint8_t*)"can't close the file \n");
        return 2;
    }
    return 0;
}