#ifndef _SHAREMEM_H
#define _SHAREMEM_H

#include <stdio.h>
#include <sys/shm.h>
#include "Setting.h"
#include "cfginfo.h"
#include <sl_types.h>

#if 0
typedef struct{
	unsigned char ucUpdateFlag; //1:updated
	unsigned int uuid;
	
	RUN_STATUS sm_run_status;
	ETH_SETTING sm_eth_setting;
	WLAN_SETTING sm_wlan_setting;
	ENCODER_SETTING sm_encoder_setting;
	RTMP_SETTING sm_rtmp_setting;
	RTSP_SETTING sm_rtsp_setting;
}SHARE_MEM;
SHARE_MEM *share_mem;
#endif

typedef struct _SHARE_MEM_{
	unsigned char ucUpdateFlag; //1:updated
	unsigned char ucIpAddress;
	unsigned int uuid;
	
	ETH_SETTING sm_eth_setting;
	TX_INFORMATION_S sm_tx_info;
} SHARE_MEM_S, *pSHARE_MEM_S;
pSHARE_MEM_S share_mem;

int InitShareMem(void);
SL_POINTER  sharemem_handle(SL_POINTER Args);

#endif
