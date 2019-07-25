#ifndef __SHAREMEM_H_
#define __SHAREMEM_H

#include <stdio.h>
#include <sys/shm.h>
#include <sl_types.h>
#include "Setting.h"
#include "cfginfo.h"
#if 0
typedef struct{
	unsigned char ucUpdateFlag; //1:updated
	unsigned char ucModeApplyFlag; //1:apply
	unsigned char ucInfoDisplayFlag; //1:send flag of display information to RX
	unsigned char ucCurrentMode;
	RUN_STATUS sm_run_status;
	ETH_SETTING sm_eth_setting;
	WLAN_SETTING sm_wlan_setting;
	ENCODER_SETTING sm_encoder_setting;
	RTMP_SETTING sm_rtmp_setting;
	RTSP_SETTING sm_rtsp_setting;
	GROUP_PACK_S sm_group_pack; //wang
	GROUP_RENAME_S sm_group_rename;
	MODE_RENAME_S sm_mode_rename;
}SHARE_MEM;
#endif
typedef struct{
	TX_INFORMATION_S tx_info[24];
	RX_INFORMATION_S rx_info[128];
	CONFIG_INFO_S config_info;
	
}SHARE_MEM;
SHARE_MEM *share_mem;

int InitShareMem(void);
SL_POINTER  sharemem_handle(SL_POINTER Args);

#endif
