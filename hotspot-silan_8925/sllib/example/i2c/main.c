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

#include <sl_i2c.h>
#include <sl_error.h>

#define I2C_DEVICE_NAME "/dev/i2c-0"

#define I2C_RETRIES        0X0701
#define TIMEOUT            0x0702
#define I2C_RDWR        0X0707

#define I2C_M_WR        0

typedef struct i2c_msg 
{
    SL_U16 addr;
    SL_U16 flags;
#define I2C_M_TEN           0x0010
#define I2C_M_RD            0x0001
#define I2C_M_NOSTART       0x4000
#define I2C_M_REV_DIR_ADDR  0x2000
#define I2C_M_IGNORE_NAK    0x1000
#define I2C_M_NO_RD_ACK     0x0800
#define I2C_M_RECV_LEN      0x0400
    SL_U16 len;
    SL_STRING buf;
}SLI2C_msg_t;

typedef struct i2c_rdwr_ioctl_data 
{
    struct i2c_msg *msgs;
    SL_S32 nmsgs;
}SLI2C_data_t;

static SL_S32 fd;        /*device fd*/

SL_ErrorCode_t SLI2C_Write(SL_U8 DeviceAddr, SL_U8 *WriteBuffer, SL_U32 WriteLength)
{
    SLI2C_data_t i2c_data;
    SL_ErrorCode_t errCode = SL_NO_ERROR;

    if (WriteBuffer == NULL 
      || DeviceAddr < 0 || DeviceAddr > 0x7f || WriteLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    i2c_data.nmsgs = 1;
	i2c_data.msgs = malloc(sizeof(SLI2C_msg_t));
    (i2c_data.msgs[0]).len   = WriteLength;
    (i2c_data.msgs[0]).addr  = DeviceAddr;
    (i2c_data.msgs[0]).flags = I2C_M_WR;
    (i2c_data.msgs[0]).buf = malloc(sizeof(WriteLength));
	 memcpy((i2c_data.msgs[0]).buf,  WriteBuffer, WriteLength);
      
    if ((ioctl(fd, I2C_RDWR, (SL_U32)&i2c_data)) < 0)
    {
        errCode = SL_ERROR_GENERIC_IO;
    }   
    return SL_NO_ERROR; 
}

SL_ErrorCode_t SLI2C_Read(SL_U8 DeviceAddr, SL_U8 *ReadBuffer, SL_U32 ReadLength)
{
    SLI2C_data_t i2c_data;
    SL_ErrorCode_t errCode = SL_NO_ERROR;

    if (ReadBuffer == NULL 
      || DeviceAddr < 0 || DeviceAddr > 0x7f || ReadLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    i2c_data.nmsgs = 1;
	i2c_data.msgs = malloc(sizeof(SLI2C_msg_t));
    (i2c_data.msgs[0]).len   = ReadLength;
    (i2c_data.msgs[0]).addr  = DeviceAddr;
    (i2c_data.msgs[0]).flags = I2C_M_RD;
   
    if ((ioctl(fd, I2C_RDWR, (SL_U32)&i2c_data)) < 0)
    {
        errCode = SL_ERROR_GENERIC_IO;
    }
    (i2c_data.msgs[0]).buf = malloc(sizeof(ReadLength));
    memcpy(ReadBuffer, (i2c_data.msgs[0]).buf, ReadLength);
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLI2C_WriteThenRead(SL_U8 DeviceAddr, SL_U8 *WriteBuffer, SL_U32 WriteLength, SL_U8 *ReadBuffer, SL_U32 ReadLength)
{

    SLI2C_data_t i2c_data;
    SL_ErrorCode_t errCode = SL_NO_ERROR;

    if (ReadBuffer == NULL || WriteBuffer == NULL 
		|| DeviceAddr < 0 || DeviceAddr > 0x7f 
        || ReadLength < 0 || WriteLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
  
    i2c_data.nmsgs = 2;
	i2c_data.msgs = malloc(sizeof(SLI2C_msg_t));
    (i2c_data.msgs[0]).len   = WriteLength;
    (i2c_data.msgs[0]).addr  = DeviceAddr;
    (i2c_data.msgs[0]).flags = I2C_M_WR;
    (i2c_data.msgs[0]).buf = malloc(sizeof(WriteLength));
    memcpy((i2c_data.msgs[0]).buf, WriteBuffer, WriteLength);
    
    (i2c_data.msgs[1]).len   = ReadLength;
    (i2c_data.msgs[1]).addr  = DeviceAddr;
    (i2c_data.msgs[1]).flags = I2C_M_RD;
    if ((ioctl(fd, I2C_RDWR, (SL_U32)&i2c_data)) < 0)
    {
        errCode = SL_ERROR_GENERIC_IO;
    }
    (i2c_data.msgs[1]).buf = malloc(sizeof(ReadLength));
    memcpy(ReadBuffer, (i2c_data.msgs[1]).buf, ReadLength);
    
    return SL_NO_ERROR;
}

int main(void)
{
    SL_ErrorCode_t errCode;

    SL_U8 nAddr;
    SL_U8 rbuff[10];
    SL_U8 wbuff[10];
    SL_U8 daddr[2];
    SL_U8 i;

	fd = open(I2C_DEVICE_NAME, O_RDWR);
    if (fd < 0)
    {
        return SL_ERROR_UNKNOWN_DEVICE;
    }
    nAddr = 0x50;
    wbuff[0] = 0x00;
    for(i = 0; i < 6; i++)
        wbuff[i] = 'a' + i;

    for(i = 0; i < 6; i++)
        printf("%c ", wbuff[i]);
    printf("\n");

    errCode = SLI2C_Write(nAddr, wbuff, 0x06);
    if(errCode != 0)
    {
        printf("SLI2C_Write error\n");
        return -1;
    }
    daddr[0] = 0;
    errCode = SLI2C_WriteThenRead(nAddr, daddr, 0x01, rbuff, 0x06);
    if(errCode != 0)
    {
        printf("SLI2C_WriteThenRead error\n");
        return -1;
    }
    for(i = 0; i < 6; i++)
	{	
        printf(" %d", rbuff[i]);
	}
    printf("\n");

    if ((close(fd)) < 0)
     {
         errCode = SL_ERROR_INVALID_HANDLE; 
     }
    return 0;
}

