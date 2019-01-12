#ifndef _SL_PARAM_VIU_H
#define _SL_PARAM_VIU_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sl_types.h>
#include <sl_error.h>

#define MASK_NUM 4

/* we ONLY define picture format used, all unused will be deleted!*/
typedef enum _PIXEL_FORMAT_e
{  
#if 0 
	PIXEL_FORMAT_RGB_1BPP = 0,
	PIXEL_FORMAT_RGB_2BPP,
	PIXEL_FORMAT_RGB_4BPP,
	PIXEL_FORMAT_RGB_8BPP,
	PIXEL_FORMAT_RGB_444,
	PIXEL_FORMAT_RGB_4444,
	PIXEL_FORMAT_RGB_555,
	PIXEL_FORMAT_RGB_565,
	PIXEL_FORMAT_RGB_1555,

	/*  9 reserved */
	PIXEL_FORMAT_RGB_888,
	PIXEL_FORMAT_RGB_8888,
	PIXEL_FORMAT_RGB_PLANAR_888,
	PIXEL_FORMAT_RGB_BAYER,

	PIXEL_FORMAT_YUV_A422,
	PIXEL_FORMAT_YUV_A444,
#endif
//	PIXEL_FORMAT_YUV_PLANAR_422,
	PIXEL_FORMAT_YUV_PLANAR_420,
//	PIXEL_FORMAT_YUV_PLANAR_444,

//	PIXEL_FORMAT_YUV_SEMIPLANAR_422,
	PIXEL_FORMAT_YUV_SEMIPLANAR_420,
//	PIXEL_FORMAT_YUV_SEMIPLANAR_444,

//	PIXEL_FORMAT_UYVY_PACKAGE_422,
	PIXEL_FORMAT_YUYV_PACKAGE_422,
//	PIXEL_FORMAT_VYUY_PACKAGE_422,
#if 0
	PIXEL_FORMAT_YCbCr_PLANAR,
#endif
//	PIXEL_FORMAT_BUTT   
} PIXEL_FORMAT_e;
#if 0
typedef enum _VIU_DEV_TYPE_e
{
	DEV_BT656 = 0,
	DEV_BT601 ,//1
	DEV_480P_SYNC_EMBEDDED ,//2
	DEV_480P_SYNC_SEPARATE,//3
	DEV_752x460P_SYNC_SEPARATE_YC_SEPARATE,

	DEV_720P_SYNC_EMBEDDED_YC_COMPOSITE,
	DEV_720P_SYNC_SEPARATE_YC_COMPOSITE,
	DEV_720P_SYNC_EMBEDDED_YC_SEPARATE,
	DEV_720P_SYNC_SEPARATE_YC_SEPARATE, 

	DEV_1080I_SYNC_EMBEDDED_YC_COMPOSITE,
	DEV_1080I_SYNC_SEPARATE_YC_COMPOSITE,
	DEV_1080I_SYNC_EMBEDDED_YC_SEPARATE,
	DEV_1080I_SYNC_SEPARATE_YC_SEPARATE,

	DEV_1080P_SYNC_EMBEDDED_YC_COMPOSITE,
	DEV_1080P_SYNC_SEPARATE_YC_COMPOSITE,
	DEV_1080P_SYNC_EMBEDDED_YC_SEPARATE,
	DEV_1080P_SYNC_SEPARATE_YC_SEPARATE

}
VIU_DEV_TYPE_e;
#endif

typedef enum _VIU_SYNC_MODE_e
{
	SYNC_EMBEDDED,
	SYNC_SEPARATE
}
VIU_SYNC_MODE_e;

typedef enum _VIU_YC_MODE_e
{
	YC_COMPOSITE,
	YC_SEPARATE
}
VIU_YC_MODE_e;

typedef struct _VIU_OUTPUT_INFO_s
{
	SL_U32 width;
	SL_U32 height;
	SL_U32 interlaceMode; 
	SL_U32 input_vsync_polarity; 
	SL_U32 input_hsync_polarity; 
	SL_U32 HsyncHfb;
	SL_U32 VsyncOfb;
	VIU_SYNC_MODE_e sync_mode;
	VIU_YC_MODE_e yc_mode;
}
VIU_OUTPUT_INFO_s;

typedef struct _VIU_Win_Info_s
{
	SL_U32 crop_startx;
	SL_U32 crop_starty;
	SL_U32 crop_width;
	SL_U32 crop_height;
	SL_U32 des_width;
	SL_U32 des_height;
}
VIU_Win_Info_s;

typedef struct _VIU_Mask_Att_s
{
	unsigned int index;//0~3
	int enable;
	unsigned int  startX;
	unsigned int  startY;
	unsigned int  width;
	unsigned int  height;
	unsigned int color;//y cb cr

}
VIU_Mask_Att_s;


typedef struct _VIU_MASK_INFO_s
{
	VIU_Mask_Att_s mask_attrs[MASK_NUM];

}
VIU_MASK_INFO_s;

typedef struct _VIU_Sym_Info_s
{   
	unsigned int  mirror;
	unsigned int  flip;
}   
VIU_Sym_Info_s;

typedef enum _VIU_DATA_YUV_SEQ_e
{
   /*The input sequence of the second component(only contains u and v) in BT.1120 mode */
    VIU_INPUT_DATA_VUVU = 0,    
    VIU_INPUT_DATA_UVUV,        

   /* The input sequence for yuv */
    VIU_INPUT_DATA_UYVY = 0,
    VIU_INPUT_DATA_VYUY,
    VIU_INPUT_DATA_YUYV,
    VIU_INPUT_DATA_YVYU,
    
    VIU_DATA_YUV_BUTT
} VIU_DATA_YUV_SEQ_e;

/* the _ghest bit of the BT.656 timing code */
typedef enum _BT656_FIXCODE_e
{
    /* The _ghest bit of the EAV/SAV data over the
    BT.656 protocol is always 1.*/
    BT656_FIXCODE_1 = 0, 

    /* The _ghest bit of the EAV/SAV data over the
    BT.656 protocol is always 0.*/
    BT656_FIXCODE_0            
}BT656_FIXCODE_e;

/* Polarity of the field indicator bit (F) of the BT.656 timing code */
typedef enum _BT656_FIELD_NEG_e
{
    BT656_FIELD_NEG_STD = 0,      /* the standard BT.656 mode,the first filed F=0,the second filed F=1*/
    BT656_FIELD_NEG_NSTD          /* the non-standard BT.656 mode,the first filed F=0,the second filed F=1*/
}BT656_FIELD_NEG_e;

/* captrue selection of VIUdeo input */
typedef enum _VIU_CAPSEL_e
{
    VIU_CAPSEL_TOP = 0,        /* top field */
    VIU_CAPSEL_BOTTOM,        /* bottom field */
    VIU_CAPSEL_BOTH,            /* top and bottom field */
    VIU_CAPSEL_BUTT
} VIU_CAPSEL_e;

typedef struct _SLVIU_Cfg_s
{
	PIXEL_FORMAT_e storeMode_cfg;
	//VIU_DEV_TYPE_e dev_type_cfg;
	VIU_OUTPUT_INFO_s output_info;
	VIU_Win_Info_s win_info_cfg;
	VIU_MASK_INFO_s mask_info_cfg;
    VIU_Sym_Info_s sym_info_cfg;
    VIU_DATA_YUV_SEQ_e data_yuvseq_cfg;
	BT656_FIXCODE_e bt656_fixcode_cfg;
	BT656_FIELD_NEG_e bt656_field_neg_cfg;
	VIU_CAPSEL_e capsel_cfg;
	SL_U32 srcFrameRate;
	SL_U32 dstFrameRate;
}
SLVIU_Cfg_s;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SL_PARAM_VIU_H */
