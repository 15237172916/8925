#ifndef _SL_PARAM_VPRE2H264ENC_H
#define _SL_PARAM_VPRE2H264ENC_H

#include <sl_param_vpre.h>
#include <sl_param_h264enc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define G_FRAMES_NUM 4
#define VPRE2H264ENC_BIND_DSP

typedef struct _SLVPRE2H264ENC_WindowInfo_s
{
	SL_U32 startX;
	SL_U32 startY;
	SL_U32 width;
	SL_U32 height;
}
SLVPRE2H264ENC_WindowInfo_s;

typedef struct  _SLVPRE2H264ENC_MainLayer_s
{
	SL_U32 chnId;
	SL_U32 width;
	SL_U32 height;
	SL_U32 sampleMode;
	SL_U32 colorMode;
	SLVPRE2H264ENC_WindowInfo_s InWindow;
	SLVPRE2H264ENC_WindowInfo_s priOutWindow;
	SLVPRE2H264ENC_WindowInfo_s subOutWindow;

} SLVPRE2H264ENC_MainLayer_s;

typedef struct  _SLVPRE2H264ENC_Encode_s
{
	SL_U32 chnId;
	SLH264ENC_EntropyMode_e entropyMode;
	SL_U32 srcFrameRate;
    SLH264ENC_GetStream_stat_e getstream_stat;
    SL_U32 qp;
	SL_U32 rate_i_p; 
	SL_U32 rate_IDR; 
	SL_U32 SLRCEnable;
	SL_U32 bit_rate;
	SL_U32 frameRate;
	SL_U32 sub_qp;
	SL_U32 sub_rate_i_p; 
	SL_U32 sub_rate_IDR; 
	SL_U32 sub_SLRCEnable;
	SL_U32 sub_bit_rate;
	SL_U32 sub_frameRate;

} SLVPRE2H264ENC_Encode_s;

typedef struct _SLVPRE2H264ENC_Enhance_s
{
	SL_U32 chnId;
	SL_U32 hue;
	SL_U32 brightness;
	SL_U32 contrast;
	SL_U32 sat;
	SL_U32 sharp;
}
SLVPRE2H264ENC_Enhance_s;

typedef struct _SLVPRE2H264ENC_Overlay_s
{
	SL_U32 chnId;
	SL_U32 enable;
	SL_U32 alpha;
	SL_U32 format;
	SL_U32 width;
	SL_U32 height;
	enum VPreColorMode mode;
	enum VPreColorSpace space;
	SLVPRE2H264ENC_WindowInfo_s InWindow;
	SLVPRE2H264ENC_WindowInfo_s OutWindow;
}
SLVPRE2H264ENC_Overlay_s;

typedef struct _SLVPRE2H264ENC_Background_s
{
	SL_U32 chnId;
	SL_U32 r;
	SL_U32 g;
	SL_U32 b;
	SL_U32 mainLay_enable;
	SL_U32 overlay_enable;
}
SLVPRE2H264ENC_Background_s;

typedef struct _SLVPRE2H264ENC_MixerOut_s
{
	SL_U32 chnId;
	SL_U32 width;
	SL_U32 height;
	SL_U32 sub_width;
	SL_U32 sub_height;
}
SLVPRE2H264ENC_MixerOut_s;

typedef enum
{
	SET_PRI_FRAMERATE = 0,
	SET_PRI_BITRATE,
	SET_PRI_IDRINTERVAL,
	SET_PRI_IPINTERVAL,

	SET_SUB_FRAMERATE,
	SET_SUB_BITRATE,
	SET_SUB_IDRINTERVAL,
	SET_SUB_IPINTERVAL,

	GET_PRI_FRAMERATE,
	GET_PRI_BITRATE,
	GET_PRI_IDRINTERVAL,
	GET_PRI_IPINTERVAL,
	GET_PRI_RESOLUTION,

	GET_SUB_FRAMERATE,
	GET_SUB_BITRATE,
	GET_SUB_IDRINTERVAL,
	GET_SUB_IPINTERVAL,
	GET_SUB_RESOLUTION,

}VPRE2H264ENC_EXT_CMD_e;

typedef enum
{
	SET_VPRE2H264ENC_OVERLAY_CFG = 0,
	SET_VPRE2H264ENC_BACKGROUND_CFG,
	SET_VPRE2H264ENC_MIXEROUT_CFG,
	SET_VPRE2H264ENC_MAINLAYER_CFG,
	SET_VPRE2H264ENC_ENHANCE_CFG,
	SET_VPRE2H264ENC_ENCODE_CFG,

}VPRE2H264ENC_CONFIG_CMD_e;

typedef struct _vpre2venc_ext_cmd_s
{
	VPRE2H264ENC_EXT_CMD_e cmd;
	SL_U32 val;
	void *param;
	SL_U32 curChnId;
}VPRE2H264ENC_EXT_CMD_s;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_VPRE2H264ENC_H */
