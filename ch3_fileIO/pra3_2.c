#include "../apue.h"
#include <fcntl.h>

#define     SEEK_OFFSET     16384

char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";

int main(void){
    int     fd;

    if ((fd = open("./file.hole", O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE)) < 0){
        err_sys("Creat file failed.");
    }

    if(write(fd, buf1, sizeof(buf1)) != sizeof(buf1))
        err_sys("Buf1 write error");

    if (lseek(fd, SEEK_OFFSET, SEEK_CUR) == -1)
        err_sys("lseek error");

    if (write(fd, buf2, sizeof(buf2)) != sizeof(buf2))
        err_sys("buf2 write error");

    exit(0);
}
