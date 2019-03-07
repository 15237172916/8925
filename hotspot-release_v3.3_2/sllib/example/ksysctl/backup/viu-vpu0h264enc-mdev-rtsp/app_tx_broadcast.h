#ifndef __APP_TX_BROADCAST_H_
#define __APP_TX_BROADCAST_H_

#define SERV_UDP_PORT	8888
#define PROBE		0xFEDCBA


typedef struct
{
	unsigned char ucIpAddress;
	unsigned char ucMultiAddress;
	unsigned int uProbe;
	unsigned int uuid;
	unsigned char ucInfoDisplayFlag;
	
} REPORT_PACK_S;

void *control_slave();

#endif
