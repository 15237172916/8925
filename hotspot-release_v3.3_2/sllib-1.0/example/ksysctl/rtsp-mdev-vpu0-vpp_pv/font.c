#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/fs.h>
#include "font.h"
#include "gui.h"

#define EN_FNT_WIDTH  EN_WIDTH
#define EN_FNT_HEIGHT EN_HEIGHT
#define CH_FNT_WIDTH  CH_WIDTH
#define CH_FNT_HEIGHT CH_HEIGHT

static unsigned char *en_font = NULL;
static unsigned char *ch_font = NULL;

static unsigned char *font_read_fnt(const char *name)
{
    int fp;
    int ret;
    int size;
    unsigned char *data = NULL;
    struct stat st;

	fp = open(name, O_RDONLY);
    if (fp < 0)
    {
        printf("font_open_file err, open %s failed \n",name);
        return NULL;
    }
    ret = fstat(fp, &st);
    if (ret < 0) {
        log_err("Failed to 'fstat' file %d\n", fp);
        return NULL;
    }

    size = st.st_size;
    if (size <= 0)
    {
        printf("font_open_file err, file size %d \n",size);
        close(fp);
        return NULL;
    }
        printf("font_open_file file size %d \n",size);

    data = (unsigned char *)malloc(size);
    if (data == NULL)
    {
        printf("font_open_file err, malloc memory failed \n");
        close(fp);
        return NULL;
    }

    read(fp, data, size);
    close(fp); //bly FIXME

    return data;
}

int font_init()
{
    if (en_font || !(en_font = font_read_fnt(EN_FNT_NAME)))
    {
        printf("font_init en_font err \n");
        return -1;
    }
#if 0
    if (ch_font || !(ch_font = font_read_fnt(CH_FNT_NAME)))
    {
        printf("font_init ch_font err \n");
        return -2;
    }
#endif
    return 0;
}

unsigned char *font_read_english_char(unsigned char mb)
{
    int offset, size;

    size = (EN_FNT_WIDTH/8)*EN_FNT_HEIGHT;
    offset = mb*size;

    return (en_font + offset);
}

unsigned char *font_read_chinese_char(unsigned char mb, unsigned char lb)
{
    int offset, size;

    size = (CH_FNT_WIDTH/8)*CH_FNT_HEIGHT;
    offset = ((mb - 0xa1)*94 + (lb - 0xa1))*size;

    return (ch_font + offset);
}










