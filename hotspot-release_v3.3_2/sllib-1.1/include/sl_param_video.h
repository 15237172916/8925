/*
 * Video parameter definition
 */

#ifndef _SL_PARAM_VIDEO_H
#define _SL_PARAM_VIDEO_H

#include <sl_config.h>
#include <sl_types.h>
#include <sl_param_sys.h>
#include <sl_avmem.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Data type definition
 */
/* Pixel format */
typedef enum {
    FORMAT_YUV_420 = 0,
    FORMAT_YUV_411,
    FORMAT_YUV_444,
    FORMAT_YUV_422,
    FORMAT_YUV_224,
    FORMAT_YUV_400,

    FORMAT_RGB_444,
    FORMAT_RGB_565,
    FORMAT_RGB_555,
    FORMAT_RGB_1555,
    FORMAT_RGB_4444,
    FORMAT_RGB_888,
    FORMAT_RGB_8888,

    FORMAT_LUT_1BPP,
    FORMAT_LUT_2BPP,
    FORMAT_LUT_4BPP,
    FORMAT_LUT_8BPP,

    /* This should always be the last */
    FORMAT_MAX
} pixelFormat_e;

/* Type of scaling */
typedef enum {
    SCALE_X_NO_Y_NO = 0, /*NO scale*/
    SCALE_X_1_2_Y_NO,  /* XScale down to 1/2,YScale NO */
    SCALE_X_1_4_Y_NO,  /* XScale down to 1/4,YScale NO */
    SCALE_X_1_8_Y_NO,  /* XScale down to 1/8,YScale NO */
    SCALE_X_2_1_Y_NO,  /* XScale up to 2X,YScale NO */
    SCALE_X_4_1_Y_NO,   /* XScale up to 4X,YScale NO */
  
    SCALE_X_NO_Y_1_2,   /* XScale NO,YScale down to 1/2*/
    SCALE_X_1_2_Y_1_2,  /* XScale down to 1/2,YScale down to 1/2 */
    SCALE_X_1_4_Y_1_2,  /* XScale down to 1/4,YScale down to 1/2 */
    SCALE_X_1_8_Y_1_2,  /* XScale down to 1/8,YScale down to 1/2 */
    SCALE_X_2_1_Y_1_2,  /* XScale up to 2X,YScale down to 1/2 */
    SCALE_X_4_1_Y_1_2,   /* XScale up to 4X,YScale down to 1/2 */

    SCALE_X_NO_Y_1_4,   /* XScale NO,YScale down to 1/4*/
    SCALE_X_1_2_Y_1_4,  /* XScale down to 1/2,YScale down to 1/4 */
    SCALE_X_1_4_Y_1_4,  /* XScale down to 1/4,YScale down to 1/4 */
    SCALE_X_1_8_Y_1_4,  /* XScale down to 1/8,YScale down to 1/4 */
    SCALE_X_2_1_Y_1_4,  /* XScale up to 2X,YScale down to 1/4 */
    SCALE_X_4_1_Y_1_4,   /* XScale up to 4X,YScale down to 1/4 */

    SCALE_X_NO_Y_1_8,	/* XScale NO,YScale down to 1/8*/
    SCALE_X_1_2_Y_1_8,  /* XScale down to 1/2,YScale down to 1/8 */
    SCALE_X_1_4_Y_1_8,  /* XScale down to 1/4,YScale down to 1/8 */
    SCALE_X_1_8_Y_1_8,  /* XScale down to 1/8,YScale down to 1/8 */
    SCALE_X_2_1_Y_1_8,  /* XScale up to 2X,YScale down to 1/8 */
    SCALE_X_4_1_Y_1_8,   /* XScale up to 4X,YScale down to 1/8 */

    SCALE_X_NO_Y_2_1,   /* XScale NO,YScale up to 2X*/
    SCALE_X_1_2_Y_2_1,  /* XScale down to 1/2,YScale up to 2X */
    SCALE_X_1_4_Y_2_1,  /* XScale down to 1/4,YScale up to 2X */
    SCALE_X_1_8_Y_2_1,  /* XScale down to 1/8,YScale up to 2X */
    SCALE_X_2_1_Y_2_1,  /* XScale up to 2X,YScale up to 2X */
    SCALE_X_4_1_Y_2_1,   /* XScale up to 4X,YScale up to 2X */

    SCALE_X_NO_Y_4_1,   /* XScale NO,YScale up to 4X*/
    SCALE_X_1_2_Y_4_1,  /* XScale down to 1/2,YScale up to 4X */
    SCALE_X_1_4_Y_4_1,  /* XScale down to 1/4,YScale up to 4X */
    SCALE_X_1_8_Y_4_1,  /* XScale down to 1/8,YScale up to 4X */
    SCALE_X_2_1_Y_4_1,  /* XScale up to 2X,YScale up to 4X */
    SCALE_X_4_1_Y_4_1,  /* XScale up to 4X,YScale up to 4X */

    SCALE_HD1080_TO_HD720,
    SCALE_HD1080_TO_D1,
    SCALE_HD720_TO_D1,

    /* This should always be the last */
    SCALE_MAX
} scaleType_e;

/* Resolution ratio */
typedef enum {
    RESO_QCIF = 0,
    RESO_CIF,    
    RESO_2CIF,    
    RESO_HD1, 
    RESO_D1,
    RESO_4D1,
    
    RESO_QVGA,    /* 320 * 240 */
    RESO_VGA,     /* 640 * 480 */    
    RESO_XGA,     /* 1024 * 768 */   
    RESO_SXGA,    /* 1400 * 1050 */    
    RESO_UXGA,    /* 1600 * 1200 */    
    RESO_QXGA,    /* 2048 * 1536 */

    RESO_WVGA,    /* 854 * 480 */
    RESO_WSXGA,   /* 1680 * 1050 */      
    RESO_WUXGA,   /* 1920 * 1200 */
    RESO_WQXGA,   /* 2560 * 1600 */
    
    RESO_HD720,   /* 1280 * 720 */
    RESO_HD1080,  /* 1920 * 1080 */
    RESO_960H,
    RESO_480H,

    /* The user defined resolution. In this case, the member 'reso_wi' and
     * 'reso_wi' of 'chanvideoPara_s' should be configured by user */
    RESO_USER_DEFINED,

    /* This should always be the last */
    RESO_MAX
} resoRatio_e;

/* Parameters of video */
typedef struct video_para {
    pixelFormat_e format;  /* Format of the video */
    resoRatio_e reso;
    u16 reso_wi; /* Width of the resolution ratio */
    u16 reso_hi; /* Height of the resolution ratio */
} videoPara_s;

/* Video parameter of a channel */
typedef struct chan_video_para {
    /* Fields should be configured by user */
    u16 chan_id;
    ioType_e io;
    pixelFormat_e format;  /* Format of the video */
    resoRatio_e reso;
    u16 reso_wi; /* Width of the resolution ratio */
    u16 reso_hi; /* Height of the resolution ratio */
} chanVideoPara_s;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_VIDEO_H */
