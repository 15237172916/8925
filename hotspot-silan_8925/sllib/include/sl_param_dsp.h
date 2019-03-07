#ifndef _SL_PARAM_DSP_H
#define _SL_PARAM_DSP_H

#include <sl_param_mp3_enc_dsp.h>
#include <sl_param_scale_dsp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DSP_EXT_CMD_ALLOC_PARAM_BUFF = 0,
    DSP_EXT_CMD_PARAM_PROCESS,
    DSP_EXT_CMD_FREE_PARAM_BUFF,
    DSP_EXT_CMD_PRINT_BUFF_INIT,
    DSP_EXT_CMD_PRINT_CFG
}dspExtCmd_e;

typedef enum
{
	DSP_CODEC_START = 0,
	DSP_CODEC_FDIP = DSP_CODEC_START,
	DSP_CODEC_VTK,
	DSP_CODEC_MP3_ENC,
    DSP_CODEC_AAC_ENC,
	DSP_CODEC_SCALE,
	DSP_CODEC_MAX,
}DSP_CODEC_TYPE;

typedef struct _DSP_CFG
{
    DSP_CODEC_TYPE   type;
    union
	{
        DSP_MP3_ENC_CFG mp3_enc_cfg;	
        DSP_SCALE_CFG_S scale_cfg;
	}u;
}DSP_CFG;

typedef struct _dsp_ext_cmd
{
	dspExtCmd_e cmd;
	void *param;
}dspExtCmd_s;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_VTK_DSP_H */
