
/**
 * @file sl_uart.h
 * @brief provide the ports for uart devices
 * @author Chen Jianneng
 * @version SL_UART-REL_1.1.0
 * @date 2012-10-08
 */

#ifndef _SL_UART_H_
#define _SL_UART_H_    

#include <sl_types.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SLUARTTYPE
{
    COMMON,     /*common uart*/
    HIGHSPEED,  /*high speed uart*/
};

enum SLUARTDevice 
{
    SLUART1, /*uart1 or uart3*/
    SLUART2, /*uart2 or uart4*/
};

#define SLUART_Handle_t SL_U32

typedef struct _SLUART_OpenParms_t
{
    enum SLUARTTYPE   type;
    enum SLUARTDevice device;
    SL_U32            speed;  /*baund rate*/     
    SL_U32            bits;   /*7 or 8*/
    SL_U8             event;  /* O or E or N*/
    SL_U8             stop;   /*1 or 2*/
}SLUART_OpenParams_t;


SL_ErrorCode_t SLUART_Open(const SLUART_OpenParams_t *OpenParams);
SL_ErrorCode_t SLUART_Close();

SL_ErrorCode_t SLUART_Read(SL_U8 *ReadBuffer, SL_U32 ReadLength);

SL_ErrorCode_t SLUART_Write(SL_U8 *WriteBuffer, SL_U32 WriteLength);

#ifdef __cplusplus
}
#endif

#endif

