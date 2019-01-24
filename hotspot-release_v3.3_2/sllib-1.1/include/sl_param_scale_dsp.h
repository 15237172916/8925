#ifndef _SL_PARAM_SCALE_DSP_H
#define _SL_PARAM_SCALE_DSP_H

#include <sl_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	IMG_FORMAT_START = 0,
	IMG_FORMAT_YUV_420 = IMG_FORMAT_START,
	IMG_FORMAT_YUV_444,
	IMG_FORMAT_END
}IMG_FORMAT_E;

typedef struct _DSP_SCALE_CFG_S
{
	SL_U16 srcWidth;
	SL_U16 srcHeight;
	IMG_FORMAT_E srcFormat;
	SL_U16 dstWidth;
	SL_U16 dstHeight;
	IMG_FORMAT_E dstFormat;
}DSP_SCALE_CFG_S;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_SCALE_DSP_H */
