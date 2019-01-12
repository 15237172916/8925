/* 
 * Data type definition for memory device (MDEV)
 */
#ifndef _SL_MEMORYDEV_H
#define _SL_MEMORYDEV_H

/*
 * Include needed header files
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Data type definition
 */
#define MDEV_MAX_BUFBLOCK_SIZE		(64*1024*1024)

/* Parameter ID */
typedef enum {
    MDEV_PID_SETBUF = 0,
    MDEV_PID_DROPFRAME,

    /* This should always be the last item */
    MDEV_PID_MAX
} mdevParaId_t;

/* Memory accessing control */
typedef struct mem_ctrl {
	SL_U32 num;		/* Number of memory blocks */
	SL_U32 size;	/* Size of each memory block */
	SL_U32 align;	/* Aligned bytes of start address */
} memCtrl_s;

/*
 * Function declaration
 */

/**
 * SLMDEV_getBlockWrite() - Get a free memory block for writing
 *
 * @param dd: Device descriptor of MDEV
 * @param buf: Memory block that is empty
 * @param size: Size of the memory block
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 SLMDEV_getBlockWrite(SL_U32 dd, void **buf, SL_U32 *size);

/**
 * SLMDEV_returnBlockWrite() - Return a block that data has been written to 
 *
 * @param dd: Device descriptor of MDEV
 * @param buf: The memory block (returned by function MDEV_getBlockWrite)
 *             that data has been written to
 * @param dlen: Length of the written data in the block
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 *
 * @usage: Call this function after data has been witten to 'buf'
 */
SL_S32 SLMDEV_returnBlockWrite(SL_U32 dd, void *buf, SL_U32 dlen);

/**
 * SLMDEV_getBlockRead() - Get a memory block that data is reday for reading
 *
 * @param dd: Device descriptor of MDEV
 * @param buf: Memory block that data is ready
 * @param dlen: Length of valid data in the memory block
 
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 SLMDEV_getBlockRead(SL_U32 dd, void **buf, SL_U32 *dlen, SL_S32 timeout);

/**
 * SLMDEV_returnBlockRead() - Return a block that data has been read out
 *
 * @param dd: Device descriptor of MDEV
 * @param buf: The memory block (returned by function MDEV_getBlockRead)
 *             that data has been read out
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 *
 * @usage: Call this function after data has been read out from 'buf'
 */
SL_S32 SLMDEV_returnBlockRead(SL_U32 dd, void *buf);

/**
 * SLMDEV_setDropFrame() - set paramter iDropFrame to MDEV
 *
 * @param dd: Device descriptor of MDEV
 * @param iDropFrame: TRUE or FALSE
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 SLMDEV_setDropFrame(SL_U32 dd, SL_BOOL iDropFrame);

/**
 * SLMDEV_setBuffer() - config buffer to MDEV
 *
 * @param dd: Device descriptor of MDEV
 * @param iDropFrame: TRUE or FALSE
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 SLMDEV_setBuffer(SL_U32 dd, memCtrl_s *buf);

SL_S32 SLMDEV_mallocBlockWrite(SL_U32 dd, void **buf, SL_U32 size);
SL_S32 SLMDEV_freeBlockWrite(SL_U32 dd, void *buf);



#ifdef __cplusplus
}
#endif

#endif /* _SL_MEMORYDEV_H */
