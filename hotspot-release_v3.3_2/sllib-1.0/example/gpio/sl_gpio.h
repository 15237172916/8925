#ifndef _SL_GPIO_H_
#define _SL_GPIO_H_

#include <sl_types.h>
#include <sl_debug.h>
#include <sl_error.h>

#define SL_BOOL SL_U8

SL_ErrorCode_t SLGPIO_Open();
SL_ErrorCode_t SLGPIO_Close();

SL_ErrorCode_t SLGPIO_Request(SL_U8 Gpio);
SL_ErrorCode_t SLGPIO_Free(SL_U8 Gpio);
SL_ErrorCode_t SLGPIO_Direction_Input(SL_U8 Gpio, SL_U8 Value);
SL_ErrorCode_t SLGPIO_Direction_Output(SL_U8 Gpio, SL_U8 Value);
SL_ErrorCode_t SLGPIO_GetValue(SL_U8 Gpio, SL_U8 *Value);
SL_ErrorCode_t SLGPIO_SetValue(SL_U8 Gpio, SL_U8 Value);

#ifdef __cplusplus
}
#endif

#endif
