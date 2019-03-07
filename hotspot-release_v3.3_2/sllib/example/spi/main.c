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
#include <sl_spi.h>

#define SPI_DEVICE_NAME "/dev/spidev1.1"


#define SPI_CPHA        0x01
#define SPI_CPOL        0x02

#define SPI_MODE_0      (0|0)
#define SPI_MODE_1      (0|SPI_CPHA)
#define SPI_MODE_2      (SPI_CPOL|0)
#define SPI_MODE_3      (SPI_CPOL|SPI_CPHA)

#define SPI_CS_HIGH     0x04
#define SPI_LSB_FIRST   0x08
#define SPI_3WIRE       0x10
#define SPI_LOOP        0x20
#define SPI_NO_CS       0x40
#define SPI_READY       0x80

#define SPI_IOC_MAGIC   'k'
typedef struct spi_ioc_transfer 
{
    SL_U64  tx_buf;
    SL_U64  rx_buf;

    SL_U32  len;
    SL_U32  speed_hz;

    SL_U16  delay_usecs;
    SL_U8   bits_per_word;
    SL_U8   cs_change;
    SL_U32  pad;
}SL_spi_t;


#define SPI_MSGSIZE(N) \
    ((((N)*(sizeof (struct spi_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
        ? ((N)*(sizeof (struct spi_ioc_transfer))) : 0)
#define SPI_IOC_MESSAGE(N) _IOW(SPI_IOC_MAGIC, 0, char[SPI_MSGSIZE(N)])


/* Read / Write of SPI mode (SPI_MODE_0..SPI_MODE_3) */
#define SPI_IOC_RD_MODE            _IOR(SPI_IOC_MAGIC, 1, SL_U8)
#define SPI_IOC_WR_MODE            _IOW(SPI_IOC_MAGIC, 1, SL_U8)

/* Read / Write SPI bit justification */
#define SPI_IOC_RD_LSB_FIRST        _IOR(SPI_IOC_MAGIC, 2, SL_U8)
#define SPI_IOC_WR_LSB_FIRST        _IOW(SPI_IOC_MAGIC, 2, SL_U8)

/* Read / Write SPI device word length (1..N) */
#define SPI_IOC_RD_BITS_PER_WORD    _IOR(SPI_IOC_MAGIC, 3, SL_U8)
#define SPI_IOC_WR_BITS_PER_WORD    _IOW(SPI_IOC_MAGIC, 3, SL_U8)

/* Read / Write SPI device default max speed hz */
#define SPI_IOC_RD_MAX_SPEED_HZ        _IOR(SPI_IOC_MAGIC, 4, SL_U32)
#define SPI_IOC_WR_MAX_SPEED_HZ        _IOW(SPI_IOC_MAGIC, 4, SL_U32)
typedef struct 
{
    SL_S32 fd;        /*device fd*/
    SL_U16 delay;
    SL_U8  bits;
    SL_U32 speed;
}spi_handle_t;

static spi_handle_t m_spi_handle = {0};
static spi_handle_t * spi_handle = &m_spi_handle;


SL_ErrorCode_t SLSPI_SetMode(enum SLSPIMode mode)
{
    if (spi_handle == NULL || mode < 0 || mode > 3)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    
    if ((ioctl(spi_handle->fd, SPI_IOC_WR_MODE, &mode)) < 0)
    {
        return  SL_ERROR_GENERIC_IO; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_GetMode(SL_U8 *mode)
{
    if (spi_handle == NULL || mode == NULL)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((ioctl(spi_handle->fd, SPI_IOC_RD_MODE, mode)) < 0)
    {
		return  SL_ERROR_GENERIC_IO; 
    }
	return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_SetSpeed(SL_U32 speed)
{
    if (spi_handle == NULL || speed < 0 || speed > 4*1000*1000)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((ioctl(spi_handle->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)) < 0)
    {
        return  SL_ERROR_GENERIC_IO; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_GetSpeed(SL_U32 *speed)
{
    if (spi_handle == NULL || speed == NULL)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
	if ((ioctl(spi_handle->fd, SPI_IOC_RD_MAX_SPEED_HZ, speed)) < 0)
    {
		return SL_ERROR_GENERIC_IO; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_SetBits(SL_U8 bits)
{
    if (spi_handle == NULL || (bits != 8 && bits != 16 && bits != 32))
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((ioctl(spi_handle->fd, SPI_IOC_WR_BITS_PER_WORD, &bits)) < 0)
    {
        return SL_ERROR_GENERIC_IO; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_GetBits(SL_U8 *bits)
{
    if (spi_handle == NULL || bits == NULL)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
	if ((ioctl(spi_handle->fd, SPI_IOC_RD_BITS_PER_WORD, bits)) < 0)
	{
		return SL_ERROR_GENERIC_IO; 
	}
     return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_Open(const SLSPI_OpenParams_t *OpenParams)
{
    SL_ErrorCode_t errCode = SL_NO_ERROR;
    
    if ((OpenParams == NULL) || ((OpenParams->device) < 0))
	{
        return SL_ERROR_BAD_PARAMETER;
    }
    spi_handle->fd = open(SPI_DEVICE_NAME, O_RDWR);

    if (spi_handle->fd < 0)
    {
        return SL_ERROR_UNKNOWN_DEVICE;
    }

    errCode = SLSPI_SetMode(OpenParams->mode);
    if (errCode)
    {
        return errCode;
    }
    
    errCode = SLSPI_SetSpeed(OpenParams->speed);
    if (errCode)
    {
        return errCode;
    }

    errCode = SLSPI_SetBits(OpenParams->bits);
    if (errCode)
    {
        return errCode;
    }
    return SL_NO_ERROR; 
}

SL_ErrorCode_t SLSPI_Close()
{
    if ((close(spi_handle->fd)) < 0)
    {
		return  SL_ERROR_INVALID_HANDLE; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_Transfer(SL_U8          *nTx, 
                              SL_U8          *nRx, 
                              SL_U32         Length)
{
    SL_spi_t spi_tr = 
    { 
        .tx_buf = (SL_U32)(nTx),
        .rx_buf = (SL_U32)(nRx),
        .len = Length,
        //.delay_usecs = spi_handle->delay,
        //.speed_hz = spi_handle->speed,
        //.bits_per_word = spi_handle->bits,
    };
        
    if (nTx == NULL || nRx == NULL || Length < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((ioctl(spi_handle->fd, SPI_IOC_MESSAGE(1), &spi_tr)) < 1)
    {
        return SL_ERROR_GENERIC_IO; 
    }
    return SL_NO_ERROR;
} 

SL_ErrorCode_t SLSPI_Read(SL_U8          *nRx, 
                          SL_U32         Length)
{
    if (nRx == NULL || Length < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((read(spi_handle->fd, nRx, Length)) < 0)
    {
		return  SL_ERROR_GENERIC_IO; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLSPI_Write(SL_U8          *nTx, 
                          SL_U32         Length)
{
    if (nTx == NULL || Length < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((write(spi_handle->fd, nTx, Length)) < 0)
    {
		return SL_ERROR_GENERIC_IO; 
    }
    return SL_NO_ERROR;
}


int main(void)
{
    SLSPI_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;

    SL_U8 wbuff[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
    SL_U8 rbuff[12] = {0,};
    SL_U8 i;
    SL_U8 bits;
    SL_U32 speed;
    SL_U8 mode;

	nOpenParam = (SLSPI_OpenParams_t *)malloc(sizeof(SLSPI_OpenParams_t));
    nOpenParam->device = SLSPI_MASTER1;
    nOpenParam->speed = 400000;
    nOpenParam->bits = 16;
    nOpenParam->mode = SLSPI_MODE0;

    errCode = SLSPI_Open(nOpenParam);
    if(errCode != 0)
    {
        printf("SLSPI_Open error\n");
        return -1;
    }
   
    errCode = SLSPI_GetMode(&mode);
    if(errCode != 0)
    {
        printf("SLSPI_GetMode error\n");
        return -1;
    }
    printf("SPI mode:    %d\n", mode);

    errCode = SLSPI_GetSpeed(&speed);
    if(errCode != 0)
    {
        printf("SLSPI_GetSpeed error\n");
        return -1;
    }
    printf("SPI speed:    %d\n", speed);

    errCode = SLSPI_GetBits(&bits);
    if(errCode != 0)
    {
        printf("SLSPI_GetBits error\n");
        return -1;
    }
    printf("SPI bits:    %d\n", bits);

    for(i = 0; i < 12; i++)
        printf("%d ", wbuff[i]);
    printf("\n");
    
    errCode = SLSPI_Transfer(wbuff, rbuff, sizeof(wbuff));
    if(errCode != 0)
    {
        printf("SLSPI_Transfer error, errCode:    %d\n", errCode);
        return -1;
    }

    for(i = 0; i < 12; i++)
        printf("%d ", rbuff[i]);
    printf("\n");
    
    errCode = SLSPI_Write(wbuff, sizeof(wbuff));
    if(errCode != 0)
    {
        printf("SLSPI_Write error, errCode:    %d\n", errCode);
        return -1;
    }

    errCode = SLSPI_Read(rbuff, sizeof(rbuff));
    if(errCode != 0)
    {
        printf("SLSPI_Read error, errCode:    %d\n", errCode);
        return -1;
    }

    errCode = SLSPI_Close();
    if(errCode != 0)
    {
        printf("SLSPI_Close error\n");
        return -1;
    }
    
    return 0;
}

