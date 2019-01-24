#ifndef _SL_API_ISP_H
#define _SL_API_ISP_H

#include <sl_types.h>
#include <sl_error.h>
#include <sl_param_isp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

SL_S32 SLISP_config(SL_U32 dd, SLISP_Cfg_s *cfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SL_API_ISP_H */
