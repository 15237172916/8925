#ifndef _SL_API_PV_H
#define _SL_API_PV_H

#include <sl_types.h>
#include <sl_error.h>
#include <vpp/sl_param_pv.h>

#ifdef __cplusplus
extern "C"
{
#endif

SL_S32 SLVPP_PV_config(SL_U32 dd, SLVPP_PV_Cfg_s *cfg);
SL_S32  pv_show(SL_U32 phyAddr);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_PV_H */
