#ifndef __APP_BROADCAST_H_
#define __APP_BROADCAST_H_

#include "Setting.h"

#define SERV_UDP_PORT	8888
#define PROBE		0xFEDCBA




typedef struct
{
    char is_hdmi_input;
    //char video_resolution;
    unsigned short video_width;
    unsigned short video_height;
    unsigned char video_framrate;
    unsigned char audio_type;
    unsigned char audio_sample;
    unsigned char audio_ch;
    char fw_version[20];
} TX_INFORMATION_BROAD_S;


typedef enum CONTROL_SIGNAL CONTROL_SIGNAL_E;
typedef struct
{
    unsigned char osd_status;
    unsigned char tv_status;
    unsigned char video_source;
    unsigned char online_count;
    char fw_version[20];
    unsigned char data_type;
    CONTROL_DATA_S control_data;
} RX_INFORMATION_BROAD_S;


typedef struct
{
	unsigned int ucProbe;
	unsigned int uuid;
	unsigned char ucRepayType;
	unsigned char ucSignal;
	unsigned char ucIpAddress;
	TX_INFORMATION_BROAD_S tx_info_s;
	RX_INFORMATION_BROAD_S rx_info_s;
} REPORT_PACK_S;


void *control_respond();

#endif
