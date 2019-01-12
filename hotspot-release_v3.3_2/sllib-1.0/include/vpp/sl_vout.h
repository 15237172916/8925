#ifndef _SL_VOUT_H_
#define _SL_VOUT_H_

#include <sl_types.h>
#include <sl_error.h>
#include <sl_os.h>

#define SLVOUT_Handle_t SL_U32

#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _SLVOUT_InitParam_t
{
	SL_U32 open_max;	
} SLVOUT_InitParam_t;

typedef struct _SLVOUT_OpenParam_t
{
	SL_U8 block_flag;
} SLVOUT_OpenParam_t;

typedef struct _SLVOUT_TermParam_t
{
	SLVOUT_Handle_t handle;
} SLVOUT_TermParam_t;
#if 0
enum SLVOUT_SamplingMode 
{
	VOUTSamplingMode_Start = 0, 
	VOUTSamplingMode_444 = VOUTSamplingMode_Start,
	VOUTSamplingMode_411,
	VOUTSamplingMode_422_HOR,
	VOUTSamplingMode_422_VER,
	VOUTSamplingMode_420,
	VOUTSamplingMode_400,
	VOUTSamplingMode_End,
};

enum SLVOUT_ColorMode 
{
	VOUTColorMode_Start = 0,
	VOUTColorMode_LUT_1BPP = VOUTColorMode_Start,
	VOUTColorMode_LUT_2BPP,
	VOUTColorMode_LUT_4BPP,
	VOUTColorMode_LUT_8BPP,
	VOUTColorMode_16BPP_Alpha,
	VOUTColorMode_RLD,
	VOUTColorMode_TrueColor, 
	VOUTColorMode_VideoNonInterlace,
	VOUTColorMode_VideoInterlace,
	VOUTColorMode_End,
};

enum SLVOUT_ColorSpace 
{
	VOUTColorSpace_Start = 0,
	VOUTColorSpace_RGB_601_16_235 = VOUTColorSpace_Start,
	VOUTColorSpace_RGB_601_0_255, 
	VOUTColorSpace_RGB_709_16_235,
	VOUTColorSpace_RGB_709_0_255,	
	VOUTColorSpace_YUV_601, 
	VOUTColorSpace_YUV_709,
	VOUTColorSpace_End,
};
#endif
typedef struct _SLVOUT_AspectRatio_t 
{
	SL_U32 X;
	SL_U32 Y;
}SLVOUT_AspectRatio_t;

enum SLVOUT_WindowValueMode 
{
	VOUTWindowValueMode_Fixed = 1,
	VOUTWindowValueMode_Relative
};

enum SLVOUT_ScalingMode 
{
	VOUTScalingMode_Start = 0,
	VOUTScalingMode_PanScan = VOUTScalingMode_Start,
	VOUTScalingMode_LetterBox,
	VOUTScalingMode_Wide,
	VOUTScalingMode_Window,
	VOUTScalingMode_End,
};

typedef struct _SLVOUT_InitPictureAttr_t 
{
	enum SLVPP_SamplingMode SampleMode;
	enum SLVPP_ColorMode  ColorMode;
	enum SLVPP_ColorSpace ColorSpace;   
	SLVOUT_AspectRatio_t  PixelAspectRatio;
	SL_U32  width;
	SL_U32  height;
}SLVOUT_InitPictureAttr_t;

typedef struct _SLVOUT_yuvBuffer_t
{
	SL_U32 y_BufferAddr;
	SL_U32 u_BufferAddr;
	SL_U32 v_BufferAddr;
}SLVOUT_yuvBuffer_t;

typedef struct _SLVOUT_ShowPicture_t
{
	SL_S32 isUseUnionBuffer;
	union
	{
		SL_U32 total_BufferAddr;
		SLVOUT_yuvBuffer_t yuvBuffer;
	}u;
}SLVOUT_ShowPicture_t;

typedef struct _SLVOUT_InputWindow_t 
{
	SL_U32 X;
	SL_U32 Y;
	SL_U32 Width;
	SL_U32 Height;
	enum SLVOUT_WindowValueMode Mode;
}SLVOUT_InputWindow_t;

typedef struct _SLVOUT_OutputWindow_t
{
	SL_U32 X;
	SL_U32 Y;
	SL_U32 Width;
	SL_U32 Height;
	enum SLVOUT_WindowValueMode Mode;
	enum SLVOUT_ScalingMode scaleMode;
	SLVOUT_AspectRatio_t  DispRatio;
}SLVOUT_OutputWindow_t;

typedef struct _SLVOUT_IOWindow_t
{
	SLVOUT_InputWindow_t in;
	SLVOUT_OutputWindow_t out;	   
}SLVOUT_IOWindow_t;

typedef struct _SLVOUT_ScreenInfo_t
{
	SL_S32 width;
	SL_S32 height;
}SLVOUT_ScreenInfo_t;

enum SLVOUT_DeinterlaceMode
{
	VOUTDeinterlace_Start = 0,
	VOUTDeinterlace_Disable = VOUTDeinterlace_Start,
	VOUTDeinterlace_Bob,
	VOUTDeinterlace_Weave,
	VOUTDeinterlace_End,
};

enum SLVOUT_DeinterlaceNoiseMode
{
	VOUTDeinterlaceNoise_Start = 0,
	VOUTDeinterlaceNoise_TriangleRemove = VOUTDeinterlaceNoise_Start,
	VOUTDeinterlaceNoise_OnePixelRemove,
	VOUTDeinterlaceNoise_TwoPixelRemove,
	VOUTDeinterlaceNoise_ThreePixelRemove,
	VOUTDeinterlaceNoise_Disable,
	VOUTDeinterlaceNoise_End,
};

enum SLVOUT_DeinterlaceIOFormat
{
	VOUTDeinterlaceIO_Start = 0,
	VOUTDeinterlaceIO_InverseInput = VOUTDeinterlaceIO_Start,
	VOUTDeinterlaceIO_InverseOutput,
	VOUTDeinterlaceIO_None,
	VOUTDeinterlaceIO_End,
};

typedef struct _SLVOUT_DeintConfig_t
{
	enum SLVOUT_DeinterlaceMode DeintMode;
	enum SLVOUT_DeinterlaceNoiseMode NoiseMode;
	enum SLVOUT_DeinterlaceIOFormat IOFormat;
}SLVOUT_DeintConfig_t;

SL_ErrorCode_t SLVOUT_Init(const SL_ModuleName_t DeviceName, SLVOUT_InitParam_t *param);

SL_ErrorCode_t SLVOUT_Open(const SL_ModuleName_t DeviceName, SLVOUT_OpenParam_t *param, SLVOUT_Handle_t *handle);

SL_ErrorCode_t SLVOUT_Close(SLVOUT_Handle_t handle);

SL_ErrorCode_t SLVOUT_Enable(SLVOUT_Handle_t handle, SL_S32 enable);

SL_ErrorCode_t SLVOUT_Validate(SLVOUT_Handle_t handle);

SL_ErrorCode_t SLVOUT_DeintConfig(SLVOUT_Handle_t handle, SLVOUT_DeintConfig_t *config);

SL_ErrorCode_t SLVOUT_InitPictureAttr(SLVOUT_Handle_t handle, SLVOUT_InitPictureAttr_t *picture_attr);

SL_ErrorCode_t SLVOUT_IOWindow(SLVOUT_Handle_t handle, SLVOUT_IOWindow_t *window);

SL_ErrorCode_t SLVOUT_ShowPicture(SLVOUT_Handle_t handle, SLVOUT_ShowPicture_t *pic);

SL_ErrorCode_t SLVOUT_GetScreenInfo(SLVOUT_Handle_t handle, SLVOUT_ScreenInfo_t *info);

SL_Revision_t SLVOUT_GetRevision(void);

SL_ErrorCode_t SLVOUT_Term(const SL_ModuleName_t DeviceName, SLVOUT_TermParam_t *param);

SL_ErrorCode_t SLVOUT_GetPulldownResult(SLVOUT_Handle_t handle, SL_U32 *result);

SL_ErrorCode_t SLVOUT_UVInterleave(SLVOUT_Handle_t handle, SL_S32 enable);

#ifdef __cplusplus
}
#endif

#endif
