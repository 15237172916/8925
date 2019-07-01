#include <stdlib.h>
#include <stdio.h>
#include "display.h"
#include "osd.h"
#include <jpeg/jpeglib.h>
#include "gui.h"

#define MODE_PG  1
#define MODE_OSD 0

#if 1
static ICON_INFO icon_info[] =
{
    { ICON_MENU, 448,  224,  IMAGE_MENU },
    { ICON_CONNECT, 42,  28,  IMAGE_CONNECT },
    { ICON_CONTRL, 42,  28,  IMAGE_CONTRL },
};

static int icon_tbl[ICON_MAX];
#endif
static void fill_area(int x, int y, int width, int height, int color, unsigned char *data, int mode)
{
    rectParam_t rect;

    rect.color = color;
    rect.buf = data;
    rect.startX = x;
    rect.startY = y;
    rect.width = width;
    rect.height = height;
    if (rect.startX + rect.width > OSD_WIDTH || rect.startY + rect.height > OSD_HEIGHT)
        return;

    if (mode == MODE_OSD)
        osd_draw(&rect);
#if 0
    else if (mode == MODE_PG)
        pg_draw(&rect);
#endif
}

static void show_osd(int x, int y, int width, int height, unsigned char *data)
{
	fill_area(x, y, width, height, 0, data, MODE_OSD);
}
#if 0
static void show_pg(int x, int y, int width, int height, unsigned char *data)
{
    fill_area(x, y, width, height, 0, data, MODE_PG);
}
#endif
static void text_show_char(int x, int y, int width, int height, unsigned char *data, int color_fg, int color_bk)
{
	int i, j, num, size, tmp, k = 0;
	int *p, buf[CH_WIDTH*CH_HEIGHT];

	p = buf;
	num = width;
	size = ((width)*(height))/8;

	for (i = 0; i < size; i++, data++)
	{
		for (j = 7; j >= 0; j--)
		{
			tmp = (*data>>j)&0x1;
			if (tmp)
				*(p + k) = color_fg;
			else
				*(p + k) = color_bk;
			k++;
		}

		if (k >= width)
		{
			p += k;
			k = 0;
		}
	}

	p = buf;
	show_osd(x, y, width, height, (unsigned char *)p);
}

void osd_fill_area(int x, int y, int width, int height, int color)
{
    fill_area(x, y, width, height, color, NULL, MODE_OSD);
}

void text_show_gb2312(int x, int y, char *text, int color_fg, int color_bk, int max_len)
{
	int len = 0;
	unsigned char *data, *string;
#if 0
	color_fg = COLOR_FONT;
	if (hilight)
		color_bk = COLOR_HILIGHT;
	else
		color_bk = COLOR_CLEAR;
#endif
	string = (unsigned char *)text;

	while (*string)
	{
		if (*string >= 0xa1 && *(string + 1) >= 0xa1)
		{
			if (len + CH_WIDTH > max_len)
				break;

			data = font_read_chinese_char(*string, *(string + 1));
			text_show_char(x + len, y, CH_WIDTH, CH_HEIGHT, data, color_fg, color_bk);
			string += 2;
			len += CH_WIDTH;
		}
		else
		{
			if (len + EN_WIDTH > max_len)
				break;

			data = font_read_english_char(*string);
			text_show_char(x + len, y, EN_WIDTH, EN_HEIGHT, data, color_fg, color_bk);
			string ++;
			len += EN_WIDTH;
		}
	}

	if (len < max_len)
		osd_fill_area(x + len, y, max_len - len, EN_HEIGHT, color_bk);
}

void text_show(int x, int y, const char *text, int color_fg, int color_bk, int max_len)
{
    unsigned char buf[MAX_STRING_LEN] = {0};

    if (text)
        utf8_to_gb2312((unsigned char *)text, strlen(text), buf, MAX_STRING_LEN);

    text_show_gb2312(x, y, (char *)buf, color_fg, color_bk, max_len);
}

void osd_choose_show(int x,int y,int icon_id)
{
	SL_U8 *imageData;
	rectParam_t rect;
	
	imageData = osd_display_icon_data(icon_id);
	rect.color 	=0;
	rect.buf 	=imageData;
	rect.startX	=x;
	rect.startY	=y;
	rect.width	=icon_info[icon_id].width;
	rect.height	=icon_info[icon_id].height;
	if (rect.startX + rect.width > OSD_WIDTH || rect.startY + rect.height > OSD_HEIGHT)
        return;
     osd_draw(&rect);
}


int osd_display_init()
{
#if 1
	int i, j,k;
	ICON_INFO *info;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * input_file;
	JDIMENSION num_scanlines;
	int row_stride;
	JSAMPARRAY buffer;
	unsigned char * dst_buffer;
	unsigned short *dst;
	unsigned char * src;
	/*jpeg decode for logo*/ 
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* init icon */
	info = icon_info;
	j = sizeof(icon_info)/sizeof(icon_info[0]);
	for (i = 0; i < j; i++, info++)
	{
		if ((input_file = fopen(info->name, "rb")) == NULL) {
			fprintf(stderr, "can't open %s\n",info->name);
			exit(EXIT_FAILURE);
		}

		/* Specify data source for decompression */
		jpeg_stdio_src(&cinfo, input_file);

		/* Read file header, set default decompression parameters */
		(void) jpeg_read_header(&cinfo, TRUE);

		/* Start decompressor */
		(void) jpeg_start_decompress(&cinfo);

		printf("cinfo.output_width:%d\n",cinfo.output_width);
		printf("cinfo.output_height:%d\n",cinfo.output_height);
		printf("cinfo.output_components:%d\n",cinfo.output_components);

		dst_buffer = malloc(cinfo.output_width*cinfo.output_height*(cinfo.output_components + 1));
		if(NULL == dst_buffer) {
			printf("failed to malloc for icon_logo.jpg \n");
			return -1;
		}
		dst = (unsigned short *)dst_buffer;
		icon_tbl[i] = (int)dst_buffer;

		row_stride = cinfo.output_width * cinfo.output_components;
		buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
		/* Process data */

		while (cinfo.output_scanline < cinfo.output_height) {
			num_scanlines = jpeg_read_scanlines(&cinfo, buffer, 1);
#if 1
			//argb8888
			src = (unsigned char *)buffer[0];
			for(k=0; k<cinfo.output_width; k++) {
				*dst_buffer = *(src + 2);
				*(dst_buffer + 1) = *(src + 1);
				*(dst_buffer + 2) = *src;
				src += 3;
				dst_buffer +=3;
				*dst_buffer++ = 0xE0;
			}
#else
			//rgb565
			src = (unsigned char *)buffer[0];
			for(i=0; i<cinfo.output_width; i++) {
				*dst ++ = RGB888toRGB565(*(src + i*3), *(src + i*3 + 1),  *(src + i*3 + 2));  
			}

#endif
		}
		(void) jpeg_finish_decompress(&cinfo);
		fclose(input_file); 
		/*end jpeg decode */
	}
	jpeg_destroy_decompress(&cinfo);
#endif

	if (font_init())
	{
		printf("font_init failed\n");
		return -1;
	}

	printf("display_init done\n");

	return 0;
}

#if 1
SL_U8 *osd_display_icon_data(int id)
{
    if (id >= ICON_START && id < ICON_MAX)
        return (SL_U8 *)(icon_tbl[id]);

    return NULL;
}
#endif






