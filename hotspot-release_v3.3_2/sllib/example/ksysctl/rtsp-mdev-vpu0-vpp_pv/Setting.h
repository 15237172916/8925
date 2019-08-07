#ifndef _SETTING_H_
#define _SETTING_H_
#include "../version.h"

#define HD_VERSION "HW11.2"
#define SW_VERSION WEB_VERSION //version number is smaller than 20 byte

#define RTMP_ENABLE 1
#define RTMP_DISABLE 0
#define RTSP_ENABLE 1
#define RTSP_DISABLE 0
#define SDI_IN 2
#define HDMI_IN 1
#define NONE 0
#define WLAN_DHCP_ENABLE 1
#define WLAN_DHCP_DISABLE 0
#define INTERFACE_WLAN0 1
#define INTERFACE_ETH0 0

#define WPA 1
#define NO_EYP 0

#define DISABLE 0
#define ENABLE 1


typedef struct{
    char strEthIp[20];
    char strEthMask[20];
    char strEthGateway[20];
    char strEthMulticast[20];
}ETH_SETTING;
ETH_SETTING eth_setting;

/*
**
*/
#define UART    0
#define RS232   1
#define IR      2
#define START		0
#define HEART		1
#define RESPOND		2
#define TX			1
#define RX			0
#define ON			1
#define OFF 		0
#define ASCII       0
#define HEX         1

enum BAUD_RATE
{
    BAUD_9600   = 0,
    BAUD_115200 = 1
};
typedef enum BAUD_RATE BAUD_RATE_E;


typedef struct
{
    char is_hdmi_input;
    unsigned short video_width;
    unsigned short video_height;
    unsigned char video_framrate;
    unsigned char audio_type;
    unsigned char audio_sample;
    unsigned char audio_ch;
    char fw_version[20];
} TX_INFORMATION_S;

typedef struct
{
    BAUD_RATE_E baud_rate;
    unsigned char data_bit;
    unsigned char parity_bit;
    unsigned char stop_bit;
    unsigned char data_format;
    char on_data[128];
    char off_data[128];
} CONTROL_DATA_S;

typedef struct
{
    unsigned char osd_status;
    unsigned char tv_status;
    unsigned char video_source;
    unsigned char online_count;
    char fw_version[20];
    unsigned char data_type;
    CONTROL_DATA_S control_data;
} RX_INFORMATION_S;




#endif
