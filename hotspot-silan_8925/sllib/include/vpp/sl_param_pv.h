#ifndef _SL_PARAM_PV_H_
#define _SL_PARAM_PV_H_

#include <sl_types.h>
#include <sl_error.h>
#include <sl_os.h>
#include <sl_param_vpp.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct  _SLVPP_PV_Cfg
{
	enum SLVPP_SamplingMode sampleMode;
	enum SLVPP_ColorMode colorMode;
	SL_U32 picWidth;
	SL_U32 picHeight;
	SL_U32 src_picWidth;
	SL_U32 src_picHeight;
	enum SLVPP_WindowValueMode blkWindowMode;
	SL_U32 blkStartX;
	SL_U32 blkStartY;
	SL_U32 blkWidth;
	SL_U32 blkHeight;
	enum SLVPP_WindowValueMode outWindowMode;
	SL_U32 outStartX;
	SL_U32 outStartY;
	SL_U32 outWidth;
	SL_U32 outHeight;
	SL_U8 refreshRate;
	SL_U8 interlace;
	SL_S32 sharp;
	SL_S32 con;
	SL_S32 hue;
	SL_S32 bri;
	SL_S32 sat;

	SL_U32 fs;
	SL_U32 chns;
	SL_U32 audio_bits;
}
SLVPP_PV_Cfg_s;

#ifdef __cplusplus
}
#endif

#endif
