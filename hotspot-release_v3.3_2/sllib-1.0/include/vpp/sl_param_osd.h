#ifndef _SL_PARAM_OSD_H_
#define _SL_PARAM_OSD_H_

#include <sl_types.h>
#include <sl_error.h>
#include <sl_os.h>
#include <sl_param_vpp.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct  _SLVPP_OSD_Cfg
{
	enum SLVPP_ColorFormat colorFormat;
	enum SLVPP_ColorMode colorMode;
	SL_U32 picWidth;
	SL_U32 picHeight;
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
}
SLVPP_OSD_Cfg_s;

#ifdef __cplusplus
}
#endif

#endif
