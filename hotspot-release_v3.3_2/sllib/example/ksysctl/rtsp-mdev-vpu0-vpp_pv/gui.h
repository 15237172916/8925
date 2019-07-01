#ifndef _GUI_H
#define _GUI_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sl_types.h>

typedef struct
{
    SL_U32 startX;
    SL_U32 startY;
    SL_U32 width;
    SL_U32 height;
    SL_U32 color;
    unsigned char *buf;
}rectParam_t;

#define OSD_WIDTH       640
#define OSD_HEIGHT      360

#define MAX_STRING_LEN 256

#define EN_WIDTH    8
#define EN_HEIGHT   16

#define CH_WIDTH    32
#define CH_HEIGHT   32
#define ICON_WIDTH  24*2
#define ICON_HEIGHT 24*2

#define COLOR_CLEAR   0x00FFFFFF

#define COLOR_FONT    0xFF000000
//#define COLOR_HILIGHT 0xFFEEC900
#define COLOR_HILIGHT 0xFFFFFFFF

#define COLOR_BLACK_SOLID    0xFF000000 //agrb
#define COLOR_BLUE_SOLID	 0xFF0000FF
//#define COLOR_WHITE_SOLID    0x00FFFFFF

#define COLOR_WHITE_TRANSPARENT    0x00FFFFFF
#define COLOR_WHITE_SOLID    0xFFFFFFFF

#define GRAY			0xE0EAEAEA

#define NULL_E "Null"
#define NULL_C "空"

#if 0
#define EN_FNT_NAME "./en8_16.fnt"
#define CH_FNT_NAME "./ch.fnt"
#else
#define EN_FNT_NAME "/user/en8_16.fnt"
#define CH_FNT_NAME "/user/ch.fnt"
#endif

#endif




