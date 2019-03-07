#ifndef _SL_API_VPRE2H264ENC_H
#define _SL_API_VPRE2H264ENC_H

#include <sl_param_vpre2h264enc.h>
#include <sl_api_vpre.h>
#include <sl_param_h264enc.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 SLVPRE2H264ENC_configOverlay(SL_U32 dd, SLVPRE2H264ENC_Overlay_s *cfg);
SL_S32 SLVPRE2H264ENC_configBackGround(SL_U32 dd, SLVPRE2H264ENC_Background_s *cfg);
SL_S32 SLVPRE2H264ENC_configMixerOut(SL_U32 dd, SLVPRE2H264ENC_MixerOut_s *cfg);
SL_S32 SLVPRE2H264ENC_configMainLayer(SL_U32 dd, SLVPRE2H264ENC_MainLayer_s *cfg);
SL_S32 SLVPRE2H264ENC_configEnhance(SL_U32 dd, SLVPRE2H264ENC_Enhance_s *cfg);
SL_S32 SLVPRE2H264ENC_configEncode(SL_U32 dd, SLVPRE2H264ENC_Encode_s *cfg);
SL_S32 SLVPRE2H264ENC_setFrameRate_priStream(SL_U32 dd,   SL_U32 val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_setBitRate_priStream(SL_U32 dd,     SL_U32 val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_setIDRInterval_priStream(SL_U32 dd, SL_U32 val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_setIpInterval_priStream(SL_U32 dd,  SL_U32 val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_setFrameRate_subStream(SL_U32 dd,   SL_U32 val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_setBitRate_subStream(SL_U32 dd,     SL_U32 val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_setIDRInterval_subStream(SL_U32 dd, SL_U32 val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_setIpInterval_subStream(SL_U32 dd,  SL_U32 val, SL_U32 curChnId);

SL_S32 SLVPRE2H264ENC_getFrameRate_priStream(SL_U32 dd,   SL_U32 *val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_getBitRate_priStream(SL_U32 dd,     SL_U32 *val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_getIDRInterval_priStream(SL_U32 dd, SL_U32 *val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_getIpInterval_priStream(SL_U32 dd,  SL_U32 *val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_getFrameRate_subStream(SL_U32 dd,   SL_U32 *val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_getBitRate_subStream(SL_U32 dd,     SL_U32 *val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_getIDRInterval_subStream(SL_U32 dd, SL_U32 *val, SL_U32 curChnId);
SL_S32 SLVPRE2H264ENC_getIpInterval_subStream(SL_U32 dd,  SL_U32 *val, SL_U32 curChnId);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_VPRE2H264ENC_H */
