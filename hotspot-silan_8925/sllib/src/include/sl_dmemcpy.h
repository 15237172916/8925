/**    *********************************************************************
#**
#**                         sl_dmemcpy.h
#**                        Silan Soft Project
#**
#**       (c)Copyright 2010-2014, HangZhou Silan Microelectronics Co.,Ltd
#**                        All Rights Reserved 
#**                       http://www.silan.com
#**
#**    File Name    : sl_dmemcpy.h
#**    Description  : 
#**    Modification : 2014/05/20      nixh      Create the file
#**
#**    *********************************************************************
*/

#ifndef _SL_DMA_MEMCPY_H_
#define _SL_DMA_MEMCPY_H_
#include<linux/ioctl.h>

#define DMA_MEMCPY_DEVICE_NAME		"dmemcpy1"
#define MEMCPY_IOC_MAGIC 'M'   //dma-memcpy magic number

#define MEMCPY_IOC_WR_DMA		_IOR(MEMCPY_IOC_MAGIC, 0, int) 
#define MEMCPY_IOC_WR_DMA_LLI	_IOR(MEMCPY_IOC_MAGIC, 1, int) 
#define MEMCPY_IOC_MAXNR	2

typedef struct ioctl_memcpy_info_st
{
	unsigned int dest_dpa;
	unsigned int src_dpa;
	unsigned int len;
} ioctl_memcpy_info;

#if defined(__KERNEL__) 
int dmemcpy_lli_trans(ioctl_memcpy_info *dma_arg);
int dmemcpy_init(void);
void dmemcpy_exit(void);
#endif

#endif //_SL_DMA_MEMCPY_
