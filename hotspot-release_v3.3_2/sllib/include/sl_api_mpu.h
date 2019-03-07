/* MPU API declaration */

#ifndef _SL_API_MPU_H
#define _SL_API_MPU_H

/*
 * Header files needed
 */
#include <sl_param_mpu.h>
#include <sl_api_filedev.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 SLMPU_configAlgoModule(SL_U32 dd, mpuCfg_s *cfg);
/* Functions used to set firmware parameters */
SL_S32 SLMPU_setGmm_m_fTb(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmm_m_fTB(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmm_m_fTg(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmm_v_init(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmm_v_max(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmm_v_min(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmm_i_alpha(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmmp_Tg_Tg(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setGmmp_Ts_h(SL_U32 dd, SL_U16 val);
SL_S32 SLMPU_setDilate_widthHeight(SL_U32 dd, SL_U16 width, SL_U16 height);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_MPU_H */
