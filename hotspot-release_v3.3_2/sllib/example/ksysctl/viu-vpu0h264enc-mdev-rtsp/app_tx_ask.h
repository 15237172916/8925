#ifndef __APP_ASK_H_
#define __APP_ASK_H_

#define CLI_UDP_PORT	8888
//#define PROBE		0xFEDCBA
/*

typedef struct
{
	unsigned char ucIpAddress;
	unsigned char ucMultiAddress;
	unsigned int uProbe;
	unsigned int uuid;
	unsigned char ucInfoDisplayFlag;

} REPORT_PACK_S;
*/

void *IP_broadcast_report();



#endif
