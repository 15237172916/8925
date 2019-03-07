#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <sl_types.h>
#include "font.h"
#include "utf8.h"

//#define IMAGE_SERVER_MISSING        "/etc/icon/icon_server_missing.jpg"
#define IMAGE_SERVER_MISSING        "./icon_server_missing.jpg"

enum
{
    ICON_START = 0,
    ICON_SERVER_MISSING = ICON_START,
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

SL_U8 *osd_display_icon_data(int id);
#endif

