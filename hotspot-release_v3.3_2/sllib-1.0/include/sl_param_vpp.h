#ifndef _SL_PARAM_VPP_H
#define _SL_PARAM_VPP_H
#include<sl_types.h>

#ifdef __cplusplus
extern "C"
{
#endif
enum SLVPP_WindowValueMode 
{
	VPPWindowValueMode_Fixed = 1,
	VPPWindowValueMode_Relative
};

enum SLVPP_SamplingMode 
{
	VPPSamplingMode_Start = 0, 
	VPPSamplingMode_444 = VPPSamplingMode_Start,
	VPPSamplingMode_411,
	VPPSamplingMode_422_HOR,
	VPPSamplingMode_422_VER,
	VPPSamplingMode_420,
	VPPSamplingMode_400,
	VPPSamplingMode_End,
};

enum SLVPP_ColorMode 
{
	VPPColorMode_Start = 0,
	VPPColorMode_LUT_1BPP = VPPColorMode_Start,
	VPPColorMode_LUT_2BPP,
	VPPColorMode_LUT_4BPP,
	VPPColorMode_LUT_8BPP,
	VPPColorMode_16BPP_Alpha,
	VPPColorMode_RLD,
	VPPColorMode_TrueColor, 
	VPPColorMode_VideoNonInterlace,
	VPPColorMode_VideoInterlace,
	VPPColorMode_End,
};

enum SLVPP_ColorFormat 
{
	VPPColorFormat_Start = 0,
	VPPColorFormat_Index = VPPColorFormat_Start,	
	VPPColorFormat_16BPP_444,
	VPPColorFormat_16BPP_565,
	VPPColorFormat_16BPP_555,	
	VPPColorFormat_16BPP_1555,
	VPPColorFormat_16BPP_4444,		
	VPPColorFormat_32BPP_888,
	VPPColorFormat_32BPP_8888,
	VPPColorFormat_DVDSPU_2BIT,
	VPPColorFormat_DVBSPU_2BIT,
	VPPColorFormat_DVBSPU_4BIT,
	VPPColorFormat_DVBSPU_8BIT,
	VPPColorFormat_End,
};

enum SLVPP_ColorSpace 
{
	VPPColorSpace_Start = 0,
	VPPColorSpace_RGB_601_16_235 = VPPColorSpace_Start,
	VPPColorSpace_RGB_601_0_255, 
	VPPColorSpace_RGB_709_16_235,
	VPPColorSpace_RGB_709_0_255,	
	VPPColorSpace_YUV_601, 
	VPPColorSpace_YUV_709,
	VPPColorSpace_End,
};

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_VPRE_H */
