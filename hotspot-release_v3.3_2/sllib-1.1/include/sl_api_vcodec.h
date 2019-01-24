#ifndef _SL_API_VCODEC_H
#define _SL_API_VCODEC_H

#include <sl_types.h>
#include <sl_param_vcodec.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 SLVPUDEC_config(SL_U32 dd, SLVDEC_Cfg_s *cfg);
SL_S32 SLVPUENC_config(SL_U32 dd, SLVENC_Cfg_s *cfg);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_VCODEC_H */
