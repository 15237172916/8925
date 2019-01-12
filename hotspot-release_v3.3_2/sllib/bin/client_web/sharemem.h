#include <stdio.h>
#include <sys/shm.h>
#include "Setting.h"


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

typedef struct{
unsigned int uiFileLen;
unsigned int uiWriteLen;
}SHARE_UPLOAD;

SHARE_UPLOAD *share_upload;

int InitShareMem(void);
int InitShareMemUpload(void);

