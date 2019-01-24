#ifndef _SL_API_MP3_ENC_DSP_H
#define _SL_API_MP3_ENC_DSP_H

#include <sl_types.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 SLAACENC_config(SL_U32 dd, SL_U32 sample_rate, SL_U32 channels, SL_U32 outBitRate);

#ifdef __cplusplus
}
#endif

#endif /* _SL_API_MP3_ENC_DSP_H */
