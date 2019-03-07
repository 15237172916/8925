#ifndef _SL_PARAM_VPRE_H
#define _SL_PARAM_VPRE_H

#ifdef __cplusplus
extern "C"
{
#endif

enum VPreSamplingMode 
{
	VPreSamplingMode_Start = 0, 
	VPreSamplingMode_444 = VPreSamplingMode_Start,
	VPreSamplingMode_411,
	VPreSamplingMode_422_HOR,
	VPreSamplingMode_422_VER,
	VPreSamplingMode_420,
	VPreSamplingMode_400,
	VPreSamplingMode_End,
};

enum VPreColorMode 
{
	VPreColorMode_Start = 0,
	VPreColorMode_LUT_1BPP = VPreColorMode_Start,
	VPreColorMode_LUT_2BPP,
	VPreColorMode_LUT_4BPP,
	VPreColorMode_LUT_8BPP,
	VPreColorMode_16BPP_Alpha,
	VPreColorMode_RLD,
	VPreColorMode_TrueColor, 
	VPreColorMode_VideoNonInterlace,
	VPreColorMode_VideoInterlace,
	VPreColorMode_End,
};

enum VPreColorSpace 
{
	VPreColorSpace_Start = 0,
	VPreColorSpace_RGB_601_16_235 = VPreColorSpace_Start,
	VPreColorSpace_RGB_601_0_255, 
	VPreColorSpace_RGB_709_16_235,
	VPreColorSpace_RGB_709_0_255,	
	VPreColorSpace_YUV_601, 
	VPreColorSpace_YUV_709,
	VPreColorSpace_End,
};

enum VPreColorFormat 
{
	VPreColorFormat_Start = 0,
	VPreColorFormat_Index = VPreColorFormat_Start,	
	VPreColorFormat_16BPP_444,
	VPreColorFormat_16BPP_565,
	VPreColorFormat_16BPP_555,	
	VPreColorFormat_16BPP_1555,
	VPreColorFormat_16BPP_4444,		
	VPreColorFormat_32BPP_888,
	VPreColorFormat_32BPP_8888,
	VPreColorFormat_DVDSPU_2BIT,
	VPreColorFormat_DVBSPU_2BIT,
	VPreColorFormat_DVBSPU_4BIT,
	VPreColorFormat_DVBSPU_8BIT,
	VPreColorFormat_End,
};

enum VPreSetCfgID
{
    SET_VPRE_MAINLAYER_CFG,
    SET_VPRE_OVERLAYER_CFG,
    SET_VPRE_MIXEROUT_CFG,
    SET_VPRE_BACKGROUND_CFG,
    SET_VPRE_ENHANCE_CFG,
};

struct VPRE_PV_OutputWindow_s
{
	SL_U32 X;
	SL_U32 Y;
	SL_U32 Width;
	SL_U32 Height;
};

struct VPRE_OUT_WindowInfo_s
{
	SL_S32 width;
	SL_S32 height;
	SL_S32 uv_inter;
	enum VPreSamplingMode samplingMode;
};

typedef struct _SLVPRE_WindowInfo_s
{
	SL_U32 startX;
	SL_U32 startY;
	SL_U32 width;
	SL_U32 height;
}
SLVPRE_WindowInfo_s;

typedef struct  _SLVPRE_MainLayer_s
{
	SL_S32 chnId;
	SL_U32 width;
	SL_U32 height;
	SL_U32 sampleMode;
	SL_U32 colorMode;
	SLVPRE_WindowInfo_s InWindow;
	SLVPRE_WindowInfo_s OutWindow;

} SLVPRE_MainLayer_s;

typedef struct _SLVPRE_Enhance_s
{
	SL_S32 chnId;
	SL_U32 hue;
	SL_U32 brightness;
	SL_U32 contrast;
	SL_U32 sat;
	SL_U32 sharp;
}
SLVPRE_Enhance_s;

typedef struct _SLVPRE_Overlay_s
{
	SL_S32 chnId;
	SL_U32 enable;
	SL_U32 alpha;
	SL_U32 format;
	SL_U32 width;
	SL_U32 height;
	enum VPreColorMode mode;
	enum VPreColorSpace space;
	SL_POINTER src; //the virt pointer of osd src data 
	SL_U32 size; //the size of osd src data
	SLVPRE_WindowInfo_s InWindow;
	SLVPRE_WindowInfo_s OutWindow;
}
SLVPRE_Overlay_s;

typedef struct _SLVPRE_Background_s
{
	SL_S32 chnId;
	SL_S32 r;
	SL_S32 g;
	SL_S32 b;
	SL_S32 mainLay_enable;
	SL_S32 overlay_enable;
}
SLVPRE_Background_s;

typedef struct _SLVPRE_MixerOut_s
{
	SL_S32 chnId;
	SL_U32 width;
	SL_U32 height;
	enum VPreSamplingMode samplingMode;
}
SLVPRE_MixerOut_s;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_VPRE_H */
