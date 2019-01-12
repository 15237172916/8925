/* Data type definition of MPU parameter processing */

#ifndef _SL_PARAM_MPU_H
#define _SL_PARAM_MPU_H

/*
 * Include needed header files
 */
#include <sl_param_sys.h>
#include <sl_param_video.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Data type definition
 */
/* Module type */
typedef enum {
    MODULE_NONE = 0,
    MODULE_GMM,
    MODULE_GMMP,
    MODULE_ERODE,
    MODULE_DILATE,
    MODULE_NUM = MODULE_DILATE
} mpuModule_e;

/* Parameter ID */
typedef enum {
    MPU_PID_ALGO_MODU = 0,
    MPU_PID_GMM,
    MPU_PID_GMMP,
    MPU_PID_ERODE,
    MPU_PID_DILATE,

    /* This should always be the last item */
    MPU_PID_MAX
} mpuParaId_t;

/* MPU configuration parameters */
typedef struct cfg_para {
    mpuModule_e mt_list[MODULE_NUM]; /* List of module type */
} mpuCfg_s;

/*
 * Data types for MPU firmware
 */

/* GMM parameter ID
 * Note: when item is added or deleted, please update table "gmm_fwpara_asm"
 * in gmm.c as well */
typedef enum {
    GMM_PID_m_fTb = 0,
    GMM_PID_m_fTB,
    GMM_PID_m_fTg,
    GMM_PID_v_init,
    GMM_PID_v_max,
    GMM_PID_v_min,
    GMM_PID_i_alpha,
    GMM_PID_s_alpha_hi,
    GMM_PID_s_alpha_lo,

    /* This should always be the last item */
    GMM_PID_MAX
} gmmParaId_t;

/* GMM+ parameter ID
 * Note: when item is added or deleted, please update table "gmmp_fwpara_asm"
 * in gmmp.c as well */
typedef enum {
    GMMP_PID_Tg_Tg = 0,
    GMMP_PID_Ts_h,

    /* This should always be the last item */
    GMMP_PID_MAX
} gmmpParaId_t;

/* Dilate parameter ID */
typedef enum {
    DILATE_PID_WI = 0,  /* Width */
    DILATE_PID_HI,      /* Height */

    /* This should always be the last item */
    DILATE_PID_MAX
} dilateParaId_t;

/* MPU firmware parameter unit */
typedef struct mpu_fw_para {
    u16 id;
    u16 val;
} mpuFwPara_t;

/* MPU firmware parameter buffer */
#define MPU_PID_MAX MAX(MAX((int)GMM_PID_MAX, (int)DILATE_PID_MAX), (int)GMMP_PID_MAX)
typedef mpuFwPara_t mpuFwParaBuf_t[MPU_PID_MAX];

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_MPU_H */
