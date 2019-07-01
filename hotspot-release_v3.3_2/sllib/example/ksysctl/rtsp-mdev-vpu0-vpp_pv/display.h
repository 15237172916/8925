#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <sl_types.h>
#include "font.h"
#include "utf8.h"

#if 0
#define IMAGE_MENU				 "./icon/menu.jpg"
#define IMAGE_CONNECT		 "./icon/connect.jpg" 
#define IMAGE_CONTRL			 "./icon/contrl.jpg"
#else
#define IMAGE_MENU				 "/user/icon/menu.jpg"
#define IMAGE_CONNECT		 "/user/icon/connect.jpg" 
#define IMAGE_CONTRL			 "/user/icon/contrl.jpg"
#endif
enum
{
    ICON_START = 0,
    ICON_MENU = ICON_START,
    ICON_CONNECT,
    ICON_CONTRL,
    ICON_MAX
}ICON_ID;

typedef struct
{
    int id;
    int width;
    int height;
    char *name;
}ICON_INFO;

int osd_display_init();

extern void text_show(int x, int y, const char *text, int color_fg, int color_bk, int max_len);
extern void text_show_gb2312(int x, int y, char *text,  int color_fg, int color_bk, int max_len);
extern void icon_show(int x, int y, int icon_id);
extern void logo_show();
extern void osd_fill_area(int x, int y, int width, int height, int color);
extern void osd_choose_show(int x,int y,int icon_id);

SL_U8 *osd_display_icon_data(int id);
#endif

