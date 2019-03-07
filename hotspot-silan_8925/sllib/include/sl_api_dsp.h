#ifndef _SL_API_DSP_H
#define _SL_API_DSP_H

#include <sl_types.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 SLDSP_configPrintInit(SL_U32 dd);
SL_S32 SLDSP_configPrintEnable(SL_U32 dd, SL_BOOL enable);
SL_S32 SLDSP_configParamBuffAlloc(SL_U32 dd, SL_U32 size, void **pAddr);
SL_S32 SLDSP_configParamBuffFree(SL_U32 dd);
SL_S32 SLDSP_configParamBuffProcess(SL_U32 dd);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_DSP_H */
