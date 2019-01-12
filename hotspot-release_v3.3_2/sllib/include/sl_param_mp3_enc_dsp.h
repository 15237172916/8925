#ifndef _SL_PARAM_MP3_ENC_DSP_H
#define _SL_PARAM_MP3_ENC_DSP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DSP_MP3_ENC_CFG
{
    unsigned int    sample_rate;
    unsigned int    channels;
    unsigned int    outBitRate; 
}DSP_MP3_ENC_CFG;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_MP3_ENC_DSP_H */
