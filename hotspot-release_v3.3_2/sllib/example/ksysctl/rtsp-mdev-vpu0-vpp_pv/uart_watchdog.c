#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <sl_uart.h>
#include "uart_watchdog.h"

//#define UART_DEVICE_NAME    "/dev/ttyDMA0"
//#define UART_DEVICE_NAME    "/dev/ttyAMA0"
//#define UART_DEVICE_NAME    "/dev/ttyAMA1"
#define UART_DEVICE_NAME    "/dev/ttyAMA2"
//#define UART_DEVICE_NAME    "/dev/tty"

static SL_U32 fd;

int uart_watchdog(void)
{
    #if 1
    SLUART_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;

    //SL_U8 rbuff[10] = {0};
    //SL_U8 wbuff=0xCC;
    SL_U8 wbuff='a';

	nOpenParam = (SLUART_OpenParams_t *)malloc(sizeof(SLUART_OpenParams_t));
    //memset(wbuff, 0x33, sizeof(wbuff));
    //nOpenParam->speed = 115200;
    nOpenParam->speed = 9600;
    nOpenParam->bits = 8;
    nOpenParam->event = 'N';
    nOpenParam->stop = 1;
    printf("nOpenParam : %x \n", nOpenParam);
    printf("start opening uart \n");
    errCode = SLUART_Open(&fd, nOpenParam, UART_DEVICE_NAME);
    if(errCode != 0)
    {
        printf("SLUART_Open %s error\n", UART_DEVICE_NAME);
        return -1;
    }
    printf("open uart succeed \n");

    while (1)
    {
        errCode = SLUART_Write(fd, &wbuff, 1);
        if(errCode != 0)
        {
            printf("SLUART_Write error\n");
            return -1;
        }
        //printf("uart watchdog feed dog succeed \n");
        sleep(2);
    }
    errCode = SLUART_Close(fd);
    if(errCode != 0)
    {
        printf("SLUART_Close error\n");
        return -1;
    }
    #endif

    return 0;
}

