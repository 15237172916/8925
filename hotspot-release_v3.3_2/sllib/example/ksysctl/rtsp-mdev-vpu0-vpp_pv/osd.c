#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "osd.h"

static void * show_buf;

SL_S32 drawIndex1BPP(SL_POINTER dst, SL_U32 srcWidth, SL_U32 srcHeight, rectParam_t *rectParam)
{
    SL_U32 width;
    SL_U32 height;
    SL_U32 copyBytes;
    SL_U32 strideBytes;
    SL_POINTER src;
    SL_POINTER dst_start;
    SL_S32 i;
    SL_S32 j;
    SL_U32 * dst_int;

    width = rectParam->width;
    height = rectParam->height;
    dst_start = dst + (rectParam->startY * srcWidth + rectParam->startX)/8;
    copyBytes = width/8;
    strideBytes = srcWidth/8;

    if(rectParam->buf) {
        src = rectParam->buf;

        for(i = 0; i < height; i++) {
            memcpy(dst_start, src, copyBytes);
            src = src + copyBytes;
            dst_start = dst_start + strideBytes;
        }
    } else {
        printf("rectParam->color:%x\n",rectParam->color);
        for(i = 0; i < height; i++) {
            dst_int =dst_start;
            for(j=0;j<copyBytes/4;j++)
            {
                *dst_int = rectParam->color;
                dst_int ++;
            }
            dst_start = dst_start + strideBytes;
        }
    }

    return 0;
}

static SL_S32 drawArgb8888(SL_POINTER dst, SL_U32 srcWidth, SL_U32 srcHeight, rectParam_t *rectParam)
{
    //printf("drawArgb8888\n");
    SL_U32 width;
    SL_U32 height;
    SL_U32 copyBytes;
    SL_U32 strideBytes;
    SL_POINTER src;
    SL_POINTER dst_start;
    SL_S32 i;
    SL_S32 j;
    SL_U32 * dst_int;

    width = rectParam->width;
    height = rectParam->height;
    dst_start = dst + (rectParam->startY * srcWidth + rectParam->startX)*4;
    copyBytes = width*4;
    strideBytes = srcWidth*4;

    if(rectParam->buf) {
        src = rectParam->buf;

        for(i = 0; i < height; i++) {
            memcpy(dst_start, src, copyBytes);
            src = src + copyBytes;
            dst_start = dst_start + strideBytes;
			usleep(500); //FIXME let vpp use the ddr
        }
    } else {
        for(i = 0; i < height; i++) {
            dst_int =dst_start;
            for(j=0;j<copyBytes/4;j++)
            {
                *dst_int = rectParam->color;
                dst_int ++;
            }
            dst_start = dst_start + strideBytes;
        }
    }

    return 0;
}

SL_S32 set_osd_buf(void * buf)
{
	show_buf = buf;
	return 0;
}

SL_S32 osd_draw(rectParam_t *rectParam)
{
	drawArgb8888(show_buf, OSD_WIDTH, OSD_HEIGHT, rectParam);

	return 0;
}
