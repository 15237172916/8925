#ifndef _SL_API_JPG_H_
#define _SL_API_JPG_H_

#include <sl_types.h>
#include <sl_param_jpg.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 SLJPG_decCfg(SL_U32 dd, SLJpgDec_Cfg_s *cfg);
SL_S32 SLJPG_encCfg(SL_U32 dd, SLJpgEnc_Cfg_s *cfg);

#ifdef __cplusplus
}
#endif

#endif
