#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <sl_uart.h>


#if 1

SL_ErrorCode_t SLUART_Setopt(SL_S32 fd, 
                             SL_U32 nSpeed, 
                             SL_U32 nBits, 
                             SL_U8  nEvent, 
                             SL_U8  nStop)
{
    struct termios newtio;
    struct termios oldtio;
    //printf("fd -- %x\n", fd);
    if (tcgetattr(fd, &oldtio) != 0)
    {
        //printf("SL_ERROR_GENERIC_IO \n");
        return SL_ERROR_GENERIC_IO;
    }
    
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL |CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch(nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
        default:
            newtio.c_cflag |= CS8;
            break;
    }

    switch(nEvent)
    {
        case 'O':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
           case 'E':
            newtio.c_iflag |= (INPCK |ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
             break;
        case 'N':
             newtio.c_cflag &= ~PARENB;
             break;
        default:
             newtio.c_cflag &= ~PARENB;
            break;
    }
 
    switch(nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 230400:
            cfsetispeed(&newtio, B230400);
            cfsetospeed(&newtio, B230400);
            break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
            break;
        case 921600:
            cfsetispeed(&newtio, B921600);
            cfsetospeed(&newtio, B921600);
            break;
        case 1000000:
            cfsetispeed(&newtio, B1000000);
            cfsetospeed(&newtio, B1000000);
            break;
        case 1152000:
            cfsetispeed(&newtio, B1152000);
            cfsetospeed(&newtio, B1152000);
            break;
        case 1500000:
            cfsetispeed(&newtio, B1500000);
            cfsetospeed(&newtio, B1500000);
            break;
        case 2000000:
            cfsetispeed(&newtio, B2000000);
            cfsetospeed(&newtio, B2000000);
            break;
        case 2500000:
            cfsetispeed(&newtio, B2500000);
            cfsetospeed(&newtio, B2500000);
            break;
        case 3000000:
            cfsetispeed(&newtio, B3000000);
            cfsetospeed(&newtio, B3000000);
            break;
        case 3500000:
            cfsetispeed(&newtio, B3500000);
            cfsetospeed(&newtio, B3500000);
            break;
        case 4000000:
            cfsetispeed(&newtio, B4000000);
            cfsetospeed(&newtio, B4000000);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
           break;
    }

    if (nStop == 1)
    {
         newtio.c_cflag &= ~CSTOPB;
    }
    else if (nStop == 2)
    {
         newtio.c_cflag |= CSTOPB;
    }
    else
    {
         newtio.c_cflag &= ~CSTOPB;
    }

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    
    tcflush(fd, TCIFLUSH);
    
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        //printf("tcsetattr \n");
        return SL_ERROR_GENERIC_IO;
    }

    return SL_NO_ERROR;
}

SL_ErrorCode_t SLUART_Open(SL_U32* fd, const SLUART_OpenParams_t *OpenParams, const char *UartName)
{
    //printf("OpenParams : %x \n", OpenParams);
    if (OpenParams == NULL) 
    {
        //printf("SL_ERROR_BAD_PARAMETER \n");
        return SL_ERROR_BAD_PARAMETER;
    }
    
    *fd = open (UartName, O_RDWR);
    if (fd < 0)
    {
        //printf("SL_ERROR_UNKNOWN_DEVICE\n");
        return SL_ERROR_UNKNOWN_DEVICE;
    }
    //printf("fd : %x \n", *fd);
    if((SLUART_Setopt(*fd, OpenParams->speed, OpenParams->bits,
        OpenParams->event,OpenParams->stop)) != SL_NO_ERROR)
    {
        //printf("SLUART_Setopt\n");
        return SL_ERROR_BAD_PARAMETER;
    }
    
    return SL_NO_ERROR; 
}

SL_ErrorCode_t SLUART_Close(SL_U32 fd)
{
    if ((close(fd)) < 0)
    {
		return  SL_ERROR_INVALID_HANDLE; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLUART_Read(SL_U32 fd, SL_U8 *ReadBuffer, SL_U32 ReadLength)
{

    if (ReadBuffer == NULL || ReadLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((read(fd, ReadBuffer, ReadLength)) < 0)
	{
        return SL_ERROR_GENERIC_IO;
    }
	return SL_NO_ERROR;
}

SL_ErrorCode_t SLUART_Write(SL_U32 fd, SL_U8 *WriteBuffer, SL_U32 WriteLength)
{
    //printf("write fd : %d \n", fd);
    if (WriteBuffer == NULL || WriteLength < 0)
    {
        printf("SL_ERROR_BAD_PARAMETER\n");
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((write(fd, WriteBuffer, WriteLength)) < 0)
    {
        printf("SL_ERROR_GENERIC_IO \n");
		return SL_ERROR_GENERIC_IO;
    }
    return SL_NO_ERROR;
}
#endif