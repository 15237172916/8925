#ifndef _SL_GPIO_H_
#define _SL_GPIO_H_

#include <sl_types.h>
#include <sl_debug.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_MAXBUF           64
#define GPIO_NUM(GROUP, BIT)  (((GROUP) - 1) * 32  + (BIT))

SL_S32 GPIO_export(SL_U32 Gpio);
SL_S32 GPIO_unexport(SL_U32 Gpio);
SL_S32 GPIO_setDir(SL_U32 Gpio, SL_U32 OutFlag);
SL_S32 GPIO_setValue(SL_U32 Gpio, SL_U32 Value);
SL_S32 GPIO_getValue(SL_U32 Gpio, SL_U32 *Value);
SL_S32 GPIO_setEdge(SL_U32 Gpio, char *Edge);
SL_S32 GPIO_openFd(SL_U32 Gpio);
SL_S32 GPIO_closeFd(SL_S32 Fd);

#ifdef __cplusplus
}
#endif

#endif
