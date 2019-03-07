#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "sl_otp.h"

int main()
{
    int fd, rdlen, i, size, err;
    char *buff;
    off_t pos;

    fd = open("/dev/silan_otp", O_RDONLY);
    if (fd == -1)
        perror("open");
    printf("fd:%d\n", fd);
    /*
     * get otp's datasize by ioctl;
     */
    err = ioctl(fd, OTP_GET_SIZE, &size);
    if (err < 0) {
        printf("err=%d\n", err);
        goto close_file;
    }
    printf("size=%d\n", size);
    buff = (char *)malloc(size);
    if (buff == NULL) {
        printf("malloc failed.\n");
        goto close_file;
    }
    /*
     * lseek at the begining of device and read 100 bytes;  
     */
    pos = lseek(fd, 0, SEEK_SET);
    if (pos < 0)
        goto free_malloc;
    printf("pos:%ld\n", pos);
    rdlen = read(fd, buff, 100);
    printf("read len:%d\n", rdlen);
    for (i = 0; i < rdlen; ++i)
        printf("%x", buff[i]);
    printf("\n");

    /*
     * lseek at the medium of device and read 100 bytes;
     */
    pos = lseek(fd, 500, SEEK_SET);
    if (pos < 0)
        goto free_malloc;
    printf("pos:%ld\n", pos);
    rdlen = read(fd, buff, 100);
    printf("read len:%d\n", rdlen);
    for (i = 0; i < rdlen; ++i)
        printf("%x", buff[i]);
    printf("\n");

    /*
     * lseek at the begining of device and read all bytes;
     */
    pos = lseek(fd, 0, SEEK_SET);
    if (pos < 0)
        goto free_malloc;
    printf("pos:%ld\n", pos);
    rdlen = read(fd, buff, size);
    printf("read len:%d\n", rdlen);
    for (i = 0; i < rdlen; ++i)
        printf("%x", buff[i]);
    printf("\n");

free_malloc:
    free(buff);

close_file:
    close(fd);
    return 0;
}
