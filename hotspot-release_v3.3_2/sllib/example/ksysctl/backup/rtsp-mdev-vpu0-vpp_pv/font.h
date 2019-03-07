#ifndef _FONT_H
#define _FONT_H

extern int font_init();
extern unsigned char *font_read_icon_char(unsigned char mb);
extern unsigned char *font_read_english_char(unsigned char mb);
extern unsigned char *font_read_chinese_char(unsigned char mb, unsigned char lb);

#endif
