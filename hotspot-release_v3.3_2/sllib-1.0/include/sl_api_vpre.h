#ifndef _SL_API_VPRE_H
#define _SL_API_VPRE_H

#include <sl_types.h>
#include <sl_error.h>
#include <sl_param_vpre.h>

#ifdef __cplusplus
extern "C"
{
#endif

SL_S32 SLVPRE_configMainLayer(SL_U32 dd, SLVPRE_MainLayer_s *cfg);
SL_S32 SLVPRE_configOverlay(SL_U32 dd, SLVPRE_Overlay_s *cfg);
SL_S32 SLVPRE_configMixerOut(SL_U32 dd, SLVPRE_MixerOut_s *cfg);
SL_S32 SLVPRE_configBackground(SL_U32 dd, SLVPRE_Background_s *cfg);
SL_S32 SLVPRE_configEnhance(SL_U32 dd, SLVPRE_Enhance_s *cfg);
#ifdef __cplusplus
}
#endif

#endif /* _SL_API_VPRE_H */
