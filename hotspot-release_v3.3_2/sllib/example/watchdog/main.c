#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sl_watchdog.h>
    

int main(void)
{
    int fd, i;
    int value, count;
    char buf[] = "hello"; 

    fd = open("/dev/silan-watchdog", O_RDWR);
    if( -1 == fd)
    {
        printf("open device failed\n");
        return -1;
    }

    ioctl(fd, WDIOC_GETTIMEOUT, &value);
    printf("timeout:%d\n", value);
    i=5;
    while(i--)
    {
        ioctl(fd, WDIOC_GETTIMELEFT, &value);
        printf("timeleft:%d\n", value);
        sleep(1);
    }
    value = 85;
    ioctl(fd, WDIOC_SETTIMEOUT, &value);
    ioctl(fd, WDIOC_GETTIMEOUT, &value);
    printf("timeout:%d\n", value);
    
    while(1)
    {
        ioctl(fd, WDIOC_GETTIMELEFT, &value);
        printf("timeleft:%d\n", value);
        sleep(1);

        if(value == 20)
        {
            count = write(fd, buf, sizeof(buf));
            printf("success to write\n");
            printf("count:%d\n", count);
        }
    }
    

        return 0;
}
