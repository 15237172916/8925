#ifndef __SHAREMEM_H_
#define __SHAREMEM_H

#include <stdio.h>
#include <sys/shm.h>
#include "Setting.h"


typedef struct{
	unsigned char ucUpdateFlag; //1:updated
	RUN_STATUS sm_run_status;
	ETH_SETTING sm_eth_setting;
	WLAN_SETTING sm_wlan_setting;
	ENCODER_SETTING sm_encoder_setting;
	RTMP_SETTING sm_rtmp_setting;
	RTSP_SETTING sm_rtsp_setting;
	GROUP_PACK_S sm_group_pack; //wang
	GROUP_RENAME_S sm_group_rename;
}SHARE_MEM;

SHARE_MEM *share_mem;

typedef struct{
unsigned int uiFileLen;
unsigned int uiWriteLen;
}SHARE_UPLOAD;

SHARE_UPLOAD *share_upload;

int InitShareMem(void);
int InitShareMemUpload(void);

#endif
