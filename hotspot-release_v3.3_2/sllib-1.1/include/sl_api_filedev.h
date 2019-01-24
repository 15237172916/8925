/* 
 * Data type definition for file device
 */
#ifndef _SL_FILEDEV_H
#define _SL_FILEDEV_H

/*
 * Include needed header files
 */
#include <sl_api_ksysctl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Data type definition
 */
#define FDEV_MAX_BUFBLOCK_NUM		36
#define FDEV_MAX_BUFBLOCK_SIZE		(64*1024*1024)

/* File accessing control */
typedef struct file_ctrl {
    int fd;       /* File descripter */
    SL_U32 fsize; /* Size of the file */
    SL_U32 rptr;  /* Read offset */
    SL_U32 wptr;  /* Write offset */
    SL_U32 rlen;  /* Data length of per read */
    SL_U32 wlen;  /* Data length of per write */
} fileCtrl_s;


/*
 * Function declaration
 */
SL_S32 SLFDEV_open(SL_U32 *dd, chanVideoPara_s *cvp, SL_U32 size_cvp,
        const char *name);
SL_S32 SLFDEV_close(SL_U32 dd);
SL_S32 SLFDEV_read(SL_U32 dd);
SL_S32 SLFDEV_write(SL_U32 dd);
SL_S32 SLFDEV_setConfig(SL_U32 dd, void *cfg, SL_U32 size);
SL_S32 SLFDEV_stopProcess(SL_U32 dd);

#ifdef __cplusplus
}
#endif

#endif /* _SL_FILEDEV_H */
