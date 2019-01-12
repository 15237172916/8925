
/**
 * @file sl_i2c.h
 * @brief provide the ports for i2c devices
 * @author Chen Jianneng
 * @version SL_I2C-REL_1.1.0
 * @date 2012-10-08
 */

#ifndef _SL_I2C_H_
#define _SL_I2C_H_    

#include <sl_types.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_ErrorCode_t SLI2C_Read(SL_U8          DeviceAddr, 
                          SL_U8          *ReadBuffer, 
                          SL_U32         ReadLength);

SL_ErrorCode_t SLI2C_Write(SL_U8          DeviceAddr, 
                           SL_U8          *WriteBuffer, 
                           SL_U32         WriteLength);

SL_ErrorCode_t SLI2C_WriteThenRead(SL_U8          DeviceAddr, 
                                   SL_U8		  *WriteBuffer, 
                                   SL_U32         WriteLength, 
                                   SL_U8	     *ReadBuffer, 
                                   SL_U32         ReadLength);

#ifdef __cplusplus
}
#endif

#endif

