#ifndef __APP_BROADCAST_H_
#define __APP_BROADCAST_H_

#include "Setting.h"

#define SERV_UDP_PORT	8888
#define PROBE		0xFEDCBA

#if 0
typedef struct
{
	unsigned char ucIpAddress;
	unsigned char ucMultiAddress;
	unsigned int uProbe;
	unsigned int uuid;
	unsigned char ucInfoDisplayFlag;
	
} REPORT_PACK_S;
#endif

typedef struct
{
	unsigned int ucProbe;
	unsigned int uuid;
	unsigned char ucRepay;
	unsigned char ucCurrentState;
	TX_INFORMATION_S tx_info_s;
	RX_INFORMATION_S rx_info_s;
} REPORT_PACK_S;





void *control_slave();

#endif
