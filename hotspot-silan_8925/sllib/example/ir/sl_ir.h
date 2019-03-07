
/**
 * @file sl_ir.h
 * @brief provide the ports for ir devices
 * @author Chen Jianneng
 * @version SL_IR-REL_1.1.0
 * @date 2012-10-08
 */

#ifndef _SL_IR_H_
#define _SL_IR_H_

#include <sl_types.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SLIRDevice 
{
    SLIR_DEVICE1  /*ir*/
};

typedef struct _SLIR_OpenParms_t
{
    enum SLIRDevice device;
}SLIR_OpenParams_t;

SL_ErrorCode_t SLIR_Open(const SLIR_OpenParams_t *OpenParams);

SL_ErrorCode_t SLIR_Close();

SL_ErrorCode_t SLIR_Read(SL_U16 *irCode);

#ifdef __cplusplus
}
#endif

#endif

