#ifndef _SL_PARAM_VCODEC_H
#define _SL_PARAM_VCODEC_H

#include <sl_types.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	STD_AVC,
	STD_VC1,
	STD_MPEG2,
	STD_MPEG4,
	STD_H263,
	STD_DIV3,
	STD_RV,
	STD_AVS,
	STD_THO = 9,
    STD_VP3,
    STD_VP8,
	STD_UNKNOWN
} SLVCODEC_CodStd_e;

typedef enum {
	BASELINE_PROFILE,
	MAIN_PROFILE,
	HIGH_PROFILE
} SLVCODEC_Profile_e;

typedef struct  _SLVDEC_OpenParam
{
	SL_S32 rotAngle;
	SL_S32 mirDir;
	SL_S32 useRot;
	SL_S32 mapType;
	SL_S32 useDering;
	SL_U32 userPackNeeded;
	SLVCODEC_CodStd_e format;
	SL_U32 picWidth;
	SL_U32 picHeight;
	/* new add */
	SL_U32 avcExtension;
	SL_U32 mp4DeblkEnable;
	SL_U32 frameEndian;
	SL_U32 mpeg4Class;
	SL_U32 bitstreamMode;
	SL_U32 wtlEnable;
	SL_U32 wtlMode;
	SL_U32 bitstreamBuffer;
	SL_U32 bitstreamBufferSize;
}
SLVDEC_OpenParam_s;

typedef struct  _SLVDEC_Cfg
{
	SL_U32 chnNum;
	SLVDEC_OpenParam_s chnParam[MAX_CHANNEL_NUM];
}
SLVDEC_Cfg_s;

typedef enum {
	SET_ALL,
	SET_BITRATE,
} SLVENC_CFG_TYPE_e;

typedef struct  _SLVENC_OpenParam
{
	SLVENC_CFG_TYPE_e cfg_type;
	SLVCODEC_CodStd_e format;
	SL_S32  picWidth;
	SL_S32  picHeight;
	SL_U32  frameRateInfo;
	SL_S32  bitRate;
	SL_S32  initialDelay;
	SL_S32  vbvBufferSize;
	SL_S32  enableAutoSkip;
	SL_S32  frameSkipDisable;
	SL_S32  gopSize;
	SL_S32  idr_interval;

    SL_S32  intraCostWeight;    
    SL_S32  userMaxDeltaQp; /**< The maximum delta QP for encoding process. It works only in the H.264 mode. */
    SL_S32  userQpMin;      /**< The minimum quantized step parameter for encoding process. It works only in the H.264 mode. */
    SL_S32  userMinDeltaQp; /**< The minimum delta QP for encoding process. It works only in the H.264 mode. */

	//EncSliceMode slicemode;
	SL_S32  intraRefresh;
	SL_S32  rcIntraQp;	
	SL_S32  picQp;
	SL_S32  IdrQp;
	SL_S32  maxQp;
	SL_S32  RcBitRate;
	SL_S32  useDering;
	SL_U32  useRot; /*90 180 270*/
	SL_U32  rotAngle; /* 90 180 270*/
	SL_U32  mirDir; /* 1:hor 2:ver 3 :both*/
	SL_S32  userQpMax;
	SL_U32  userGamma;
	SL_S32  RcIntervalMode;		/* 0:normal, 1:frame_level, 2:slice_level, 3: user defined Mb_level*/
	SL_S32  MbInterval;			/* use when RcintervalMode is 3*/
	SL_S32  MESearchRange;		/* 3: 16x16, 2:32x16, 1:64x32, 0:128x64, H.263(Short Header : always 0)*/
	SL_S32  MEUseZeroPmv;		/* 0: PMV_ENABLE, 1: PMV_DISABLE*/
	SL_S32  IntraCostWeight;	/* Additional weight of Intra Cost for mode decision to reduce Intra MB density*/
	SL_S32  cbcrInterleave;
	SL_S32  drawRectEnable;
	SLVCODEC_Profile_e profile;
}
SLVENC_OpenParam_s;

typedef struct  _SLVENC_Cfg
{
	SL_U32 chnNum;
	SLVENC_OpenParam_s chnParam[MAX_CHANNEL_NUM];
}
SLVENC_Cfg_s;

typedef struct _SLVENC_ExtendStream_info
{
	SL_U32 header_offset_addr;   
	SL_U32 header_size;
	SL_U32 stream_offset_addr;
	SL_U32 stream_size;
	SL_U32 sps_size;
	SL_U32 pps_size;

}SLVENC_ExtendStream_info_s;

typedef struct _H264_APPEND_INFO
{
	//audio
	SL_U32 fs;
	SL_U32 audio_bits;
	SL_U32 chns;
	//video
	SL_U32 width;
	SL_U32 height;
	SL_U32 src_width;
	SL_U32 src_height;
	SL_U8 frameRate;
	SL_U8 refreshRate;
	SL_U8 interlace;
}
H264_APPEND_INFO_s;

typedef struct streamInfo
{
	unsigned int ptr;
	unsigned int size;
}streamInfo_s;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_VCODEC_H */
