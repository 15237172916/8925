/* 
 * Configurations of the system control module
 */

#ifndef _SL_CONFIG_H
#define _SL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * Architecture related configuration
 *********************************************************************/
#ifndef __KERNEL__
/* Disable (comment it)/enable (define it) FPGA verification */
//#define CONFIG_SILAN_FPGA
#define CONFIG_SILAN_IVS2
#endif /* __KERNEL__ */


/*********************************************************************
 * General configuration
 *********************************************************************/
/* Disable/enable debugging */
//#define SYS_DEBUG               1

/* Dump output of algorithm modules or not */
//#define DUMP_VIU_OUT            1
//#define DUMP_VPRE_OUT           1
//#define DUMP_MPU_OUT          1
//#define DUMP_VPP_OUT         1

#if defined(DUMP_VIU_OUT)   || defined(DUMP_VPRE_OUT)   || \
    defined(DUMP_MPU_OUT) || defined(DUMP_VPP_OUT)
    #define DUMP_MODULE_OUT     1
#endif

//#define DUMP_TIME_STAMP         1


/*********************************************************************
 * SYS control block related configuration
 *********************************************************************/
/* Module version */
#define SYS_CTL_V2				1

/* Memory aligning */
#define SYS_ALIGN_BYTES         8

/* Data buffer aligning */
#define SYS_BUF_ALIGN_BYTES     512

/* Maximum buffer blocks in buffer parameter */
#define BUF_PARA_NUM_BLKS       50

/* Support lock or not */
#define DEVMAN_LOCK_SUPPORT     1

/* Maximum channels of a device */
#define MAX_CHANNEL_NUM         5

/* Maximum data producers of a channel */
#define MAX_PRODUCER_NUM		5

/* Maximum data consumers of a channel */
#define MAX_CONSUMER_NUM		5

/* Maximum of device instances */
#define MAX_DEV_INSTANCE		256

/* Status info of buffer block list */
#define STAT_LIST_BLKS			(MAX_PRODUCER_NUM + 1)

#define MAX_BBPOOL_NUM         30
#define DROP_FRAME_MINNUM      8

/* Disable (comment it)/enable (define it) frame dropping if device processing
 * slower than others */
#define DROP_FRAME_SUPPORT      1
#define MEMLIST_SUPPORT         1
#define SLOS_CHECK_MEMORY

/* SYS_MEM_RING_SIZE must equal to 2^n */
#define SYS_MEM_RING_SIZE        (8*1024*1024)
//#define PRE_SIZE_MEM_RING        (800*1024)
#define PRE_SIZE_MEM_RING        (2*1024*1024)


/*********************************************************************
 * Parameters of input video related configuration
 *********************************************************************/
/* The maximum resolution ratio
 * 1920*1080/1408*1152/1280*1080/1280*720/1024*768/640*480
 */
#define MAX_RESO_WI             1920
#define MAX_RESO_HI             1088
#define MAX_RES_RTO             (MAX_RESO_WI * MAX_RESO_HI)
#define MAX_EXPD_RES_RTO        ((MAX_RESO_WI + PIXEL_EX_W) * \
                                 (MAX_RESO_HI + PIXEL_EX_H))

/* Maximum size of a frame */
#define MAX_SIZE_PER_FRAME      ((MAX_RES_RTO * 3) / 2)


/*********************************************************************
 * Functionality of modules
 *********************************************************************/
/* Enable (un-comment) of disable (comment) dynamic library loading
 * functionality */
//#define SLOS_LOAD_DLIB_SUPPORT   1


/*********************************************************************
 * Internal configurations
 *********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SL_CONFIG_H */
