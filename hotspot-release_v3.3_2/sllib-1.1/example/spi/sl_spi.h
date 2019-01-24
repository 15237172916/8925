
/**
 * @file sl_spi.h
 * @brief provide the ports for spi devices
 * @author Chen Jianneng
 * @version SL_SPI-REL_1.1.0
 * @date 2012-10-08
 */

#ifndef _SL_SPI_H_
#define _SL_SPI_H    

#include <sl_types.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SLSPIDevice 
{
    SLSPI_MASTER1, /*spi1*/
};

enum SLSPIMode 
{
    SLSPI_MODE0, /*spi mode0*/
    SLSPI_MODE1, /*spi mode1*/
    SLSPI_MODE2, /*spi mode2*/
    SLSPI_MODE3, /*spi mode3*/
};

typedef struct _SLSPI_OpenParms_t
{
    SL_U8  bits;  
	SL_U32 speed;  
	enum SLSPIMode mode;
    enum SLSPIDevice device;
}SLSPI_OpenParams_t;



SL_ErrorCode_t SLSPI_Open(const SLSPI_OpenParams_t *OpenParams);
                        
SL_ErrorCode_t SLSPI_Close();

SL_ErrorCode_t SLSPI_GetMode(SL_U8          *mode);

SL_ErrorCode_t SLSPI_GetSpeed( SL_U32          *speed);

SL_ErrorCode_t SLSPI_GetBits(SL_U8          *bits);

SL_ErrorCode_t SLSPI_Transfer(SL_U8          *nTx, 
                              SL_U8          *nRx, 
                              SL_U32         Length);

SL_ErrorCode_t SLSPI_Write(SL_U8          *nTx, 
                          SL_U32         Length);

SL_ErrorCode_t SLSPI_Read(SL_U8          *nRx, 
                          SL_U32         Length);
#ifdef __cplusplus
}
#endif

#endif

