
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#ifndef ANDROID
#include <bits/byteswap.h>
#else
#include <byteswap.h>
#endif
#include <sl_gpio.h>
#define CMD_GPIO_REQUEST 1
#define CMD_GPIO_FREE 2
#define CMD_GPIO_DIRECTION_INPUT 3
#define CMD_GPIO_DIRECTION_OUTPUT 4
#define CMD_GPIO_GET_VALUE 6
#define CMD_GPIO_SET_VALUE 5

#define GPIO_DEVICE_PATH "/dev/misc_gpio"

static SL_U32 fd;

typedef struct
{
    SL_U8 gpio;
    SL_U8 value;
}slgpio_arg_t;

static slgpio_arg_t arg ={0};

SL_ErrorCode_t mZ();

SL_ErrorCode_t SLGPIO_Open()
{
    SL_ErrorCode_t errCode = SL_NO_ERROR;
    fd = open(GPIO_DEVICE_PATH, O_RDWR|O_SYNC);
    if(fd < 0)
    {
        errCode = SL_ERROR_UNKNOWN_DEVICE;
    }
    return errCode;
}

SL_ErrorCode_t SLGPIO_Close()
{
	if(fd >=0)
		close(fd);
    return SL_NO_ERROR;	
}

SL_ErrorCode_t SLGPIO_Request(SL_U8 Gpio)
{
    arg.gpio = Gpio;
	printf("GPIO is %d\n", Gpio); 	
    ioctl(fd, CMD_GPIO_REQUEST, &arg);
	return SL_NO_ERROR;
}

SL_ErrorCode_t SLGPIO_Free(SL_U8 Gpio)
{
    arg.gpio = Gpio;
	ioctl(fd, CMD_GPIO_FREE, &arg);
	return SL_NO_ERROR;
}

SL_ErrorCode_t SLGPIO_Direction_Input(SL_U8 Gpio, SL_BOOL Value)
{
    arg.gpio = Gpio;	
    arg.value = Value;
    ioctl(fd, CMD_GPIO_DIRECTION_INPUT, &arg);
    return SL_NO_ERROR;	
}

SL_ErrorCode_t SLGPIO_Direction_Output(SL_U8 Gpio, SL_BOOL Value)
{
    arg.gpio = Gpio;	
    arg.value = Value;
    ioctl(fd, CMD_GPIO_DIRECTION_OUTPUT, &arg);
    return SL_NO_ERROR;	
}

SL_ErrorCode_t SLGPIO_GetValue(SL_U8 Gpio, SL_BOOL *Value)
{
    arg.gpio = Gpio;	
    ioctl(fd, CMD_GPIO_GET_VALUE, &arg);
    return SL_NO_ERROR;	
}

SL_ErrorCode_t SLGPIO_SetValue(SL_U8 Gpio, SL_BOOL Value)
{
    arg.gpio = Gpio;	
    arg.value = Value;
    ioctl(fd, CMD_GPIO_SET_VALUE, &arg);
    return SL_NO_ERROR;	
}

int main(void)
{
    SL_ErrorCode_t errCode;

    errCode = SLGPIO_Open();
    if(errCode != 0)
    {
        printf("SLGPIO_Open error\n");
        return -1;
    }

	return 0;

}

