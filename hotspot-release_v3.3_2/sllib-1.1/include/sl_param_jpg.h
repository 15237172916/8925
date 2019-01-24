#ifndef _SL_PARAM_JPG_H_
#define _SL_PARAM_JPG_H_

#include <sl_types.h>
#include <sl_param_video.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SET_JPG_DEC_CFG,
	SET_JPG_ENC_CFG
} SLJPG_SetCfg_e;

typedef enum {
	MIRDIR_NONE,
	MIRDIR_VER,
	MIRDIR_HOR,
	MIRDIR_HOR_VER
} SLJPG_MirrorDirection_e;

typedef enum {
	ROT_NONE,
	ROT_90 = 90,
	ROT_180 =180,
	ROT_270 = 270
} SLJPG_RotatorMode_e;

typedef struct _SLJpg_ExtendStream_info
{
	SL_U32 header_offset_addr;   
	SL_U32 header_size;
	SL_U32 stream_offset_addr;
	SL_S32 stream_size;

}SLJpg_ExtendStream_info_s;

typedef struct  _SLJpgDec_Cfg
{
    SL_U32 chnId;
    SL_U32 bEnChrIntlv;
    SL_U32 rotationEnable;
    SL_U32 mirrorEnable;
    SLJPG_MirrorDirection_e mirrorDirection;
    SLJPG_RotatorMode_e rotationAngle;
    scaleType_e scaleMode;

} SLJpgDec_Cfg_s;

typedef struct  _SLJpgEnc_Cfg
{
    SL_U32 chnId;
    pixelFormat_e mjpgChromaFormat;
    SL_U32 bEnChrIntlv;
    SL_U32 picWidth;
    SL_U32 picHeight;
    SL_U32 rotationEnable;
    SL_U32 mirrorEnable;
    SLJPG_MirrorDirection_e mirrorDirection;
    SLJPG_RotatorMode_e rotationAngle;

} SLJpgEnc_Cfg_s;

#ifdef __cplusplus
}
#endif

#endif
