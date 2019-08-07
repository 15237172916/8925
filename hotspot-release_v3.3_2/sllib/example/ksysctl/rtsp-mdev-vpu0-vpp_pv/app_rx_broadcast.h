#ifndef __APP_RX_BROADCAST_H_
#define __APP_RX_BROADCAST_H_

#include "Setting.h"

#define CLI_UDP_PORT	8888
#define PROBE		0xFEDCBA
#define START		0
#define HEART		1
#define RESPOND		2
#define SIGNAL		3
#define TX			1
#define RX			0
#define ON			1
#define OFF 		0	

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
	unsigned int uProbe;
	unsigned int uuid;
	unsigned char ucRepayType;
	unsigned char ucSignal;
	unsigned char ucIpAddress;
	TX_INFORMATION_S tx_info_s;
	RX_INFORMATION_S rx_info_s;
} REPORT_PACK_S;

void *IP_broadcast_ask(void);


#endif
