/*
 * Sys parameter definition
 */

#ifndef _SL_PARAM_SYS_H
#define _SL_PARAM_SYS_H

#include <sl_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Commonly used utilities
 */
#ifndef MIN
#define MIN(x, y)       (((x) > (y))?(y):(x))
#endif
#ifndef MAX
#define MAX(x, y)       (((x) < (y))?(y):(x))
#endif
/* The value of 'align' should be 2^n, e.g. 2, 4, 8, 16... */
#ifndef BYTES_ALIGN
#define BYTES_ALIGN(size, align)        (((size) + ((align)-1)) & (~((align)-1)))
#endif
/* The value of 'align' should be 2^n, e.g. 2, 4, 8, 16... */
#ifndef IS_BYTES_ALIGNED
#define IS_BYTES_ALIGNED(size, align)   (!((size) & ((align)-1)))
#endif


/*
 * Device ID checking
 */
/* Check if device is a MDEV */
#define IS_MDEV(id)	(ID_MEMORY == (id))

/* Check if device is VPU  */
#define IS_VPUDEC(id)	(ID_VPU0DEC == (id) || ID_VPU1DEC == (id))

/* Check if device is VIU */
#define IS_VIU(id)	((ID_VIU0 == (id)) || (ID_VIU1 == (id)) || \
		(ID_VIU2 == (id)) || (ID_VIU3 == (id)))

/* Check if device is NULLDEV */
#define IS_NDEV(id)	(ID_NULL == (id))

/* Check if device is DSP */
#define IS_DSP(id)	((ID_DSP0 == (id)) || (ID_DSP1 == (id)))


/*
 * Data type definition
 */
/* Device ID */
typedef enum {
	ID_START = 0,

	/* Devices link to DDR0 only */
	ID_GPU = ID_START, /* 0 */
	ID_VPU0DEC,
	ID_VPU0ENC,
	ID_VPU1DEC,
	ID_VPU1ENC,
	ID_VPP_PV,
	ID_VPP_SV,
	ID_VPP_PG,
	ID_VPP_SG,
	ID_VPP_OSD,
	ID_JPUDEC,
	ID_JPUENC, /* 5 */
	ID_FDIP,
	/* Devices we link them to DDR0 by force */
	ID_DSP0,
	ID_VIU0,
	ID_VIU2,
	ID_DDR0_DUMMY, /* 10 */	/* Dummy device on DDR0 */

	/* Devices link to DDR1 only */
	ID_H264ENC,
	ID_MPU,
	ID_DSP1,
	/* Devices we link them to DDR1 by force */
	ID_VIU1,
	ID_VIU3,
	ID_DDR1_DUMMY,	/* Dummy device on DDR1 */

	/* Devices link to both */
	ID_VPRE,
	ID_VPRE2H264ENC,
	ID_DEINT,
	ID_MEMORY,/* 20 */
	ID_GMAC, 
	ID_NULL,
	ID_DMA0, /* DMA0 device */
	ID_DUMMY, /* Dummy device */
	ID_DRAWRECT,
	ID_OS,
	ID_ISP,

	/* The end */
	ID_END 
} devId_e;

/* I/O type */
typedef enum {
    IO_INPUT = 0, /* Direct input */
    IO_OUTPUT,    /* Direct output */
    IO_SOURCE,    /* Original source input */
    IO_MAX
} ioType_e;

/* Buffer address */
typedef struct buf_addr {
    u32 pa; /* Physical address */
    u32 va; /* Virtual address */
} bufAddr_t;

/* Head of a parameter message */
typedef struct para_head {
    u32 para_id;    /* Parameter ID */
    u32 size;       /* Size of the parameter block pointed by "para" */
    void *para;
} paraHead_s;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_SYS_H */
