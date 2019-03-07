#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sl_led.h>


int main(void)
{
    int fd, i;
    fd = open("/dev/silan-led", O_RDONLY);
    if( -1 == fd)
    {
        printf("open device failed\n");
        return -1;
    }

    while(1)
    {
        i = 5;
        while(i--)
        {
            ioctl(fd, LED0_SET_VAL, 0);
            sleep(1);
            ioctl(fd, LED0_SET_VAL, 1);
            sleep(1);
        }

        i = 5;
        while(i--)
        {
            ioctl(fd, LED1_SET_VAL, 0);
            sleep(1);
            ioctl(fd, LED1_SET_VAL, 1);
            sleep(1);
        }

        i = 5;
        while(i--)
        {
            ioctl(fd, LED_SET_ALL);
            sleep(1);
            ioctl(fd, LED_CLEAR_ALL);
            sleep(1);
        }
    }
        return 0;
}
