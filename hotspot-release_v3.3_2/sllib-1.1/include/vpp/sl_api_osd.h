#ifndef _SL_API_OSD_H
#define _SL_API_OSD_H

#include <sl_types.h>
#include <sl_error.h>
#include <vpp/sl_param_osd.h>

#ifdef __cplusplus
extern "C"
{
#endif

SL_S32 SLVPP_OSD_config(SL_U32 dd, SLVPP_OSD_Cfg_s *cfg);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_OSD_H */
