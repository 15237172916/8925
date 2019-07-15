#ifndef __SHAREMEM_H_
#define __SHAREMEM_H

#include <stdio.h>
#include <sys/shm.h>
#include <sl_types.h>
#include "Setting.h"
#include "cfginfo.h"

typedef struct{
unsigned char ucUpdateFlag; //1:updated
RUN_STATUS sm_run_status;
ETH_SETTING sm_eth_setting;
WLAN_SETTING sm_wlan_setting;
ENCODER_SETTING sm_encoder_setting;
RTMP_SETTING sm_rtmp_setting;
RTSP_SETTING sm_rtsp_setting;
}SHARE_MEM;

SHARE_MEM *share_mem;

int InitShareMem(void);
int InitShareMem1(void);
SL_POINTER  sharemem_handle(SL_POINTER Args);

#endif
