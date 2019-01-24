/* 
 * ksysctl API declaration
 */
#ifndef _SL_API_KSYSCTL_H
#define _SL_API_KSYSCTL_H

/*
 * Include needed header files
 */
#include <sl_param_video.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function declaration
 */
SL_S32 SLSYSCTL_openSysCtlLib(void);
SL_S32 SLSYSCTL_closeSysCtlLib(void);

SL_S32 SLSYSCTL_AVMemoryCopy(void *dest, void *src, SL_U32 n);
SL_S32 SLSYSCTL_AVMemoryAlloc(SLAVMEM_Handle_t *handle, void **virtAddr, SL_U32 size);
SL_S32 SLSYSCTL_AVMemoryFree(SLAVMEM_Handle_t handle);

SL_S32 SLSYSCTL_open(SL_U32 *dd, devId_e id, chanVideoPara_s *cvp,
        SL_U32 size_cvp);
SL_S32 SLSYSCTL_close(SL_U32 dd);
SL_S32 SLSYSCTL_bind(SL_U32 pdd, SL_U32 pcid, SL_U32 cdd, SL_U32 ccid);
SL_S32 SLSYSCTL_unbind(SL_U32 pdd, SL_U32 pcid, SL_U32 cdd, SL_U32 ccid);
SL_S32 SLSYSCTL_unbindFromAll(SL_U32 cdd);
SL_S32 SLSYSCTL_setConfig(SL_U32 dd, void *cfg, SL_U32 size);
SL_S32 SLSYSCTL_getConfig(SL_U32 dd, void *cfg, SL_U32 size);
SL_S32 SLSYSCTL_prepProcess(SL_U32 dd);
SL_S32 SLSYSCTL_startProcess(SL_U32 dd);
SL_S32 SLSYSCTL_waitProcessDone(SL_U32 dd, SL_U32 ms);
SL_S32 SLSYSCTL_postProcess(SL_U32 dd);
SL_S32 SLSYSCTL_stopProcess(SL_U32 dd);
SL_S32 SLSYSCTL_getTimeoutDev(SL_U32 *dd);
void *_paToVa(SL_U32 pa);
SL_U32 _vaToPa(void *va);
SL_U32 _uvaToPa(void *uva);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_KSYSCTL_H */
