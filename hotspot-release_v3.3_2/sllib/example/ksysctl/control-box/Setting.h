#ifndef _SETTING_H_
#define _SETTING_H_
#include "../version.h"
#include "linked_list.h"

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
#if 0
typedef struct{
    unsigned char ucInputStatus; //0:none 1:HDMI 2:SDI
    unsigned short usWidth;
    unsigned short usHeight;
    unsigned short ucFrameRate;
    unsigned short usAudioSampleRate;
    unsigned char ucRTSPStatus;
    unsigned char ucRTPStatus;
    unsigned char ucRTMPStatus;
    unsigned char ucWiFiStatus;
    char strHardwareVer[20];
    char strSoftwareVer[20];
}RUN_STATUS;

RUN_STATUS run_status;

typedef struct{
    char strEthIp[20];
    char strEthMask[20];
    char strEthGateway[20];
	char strEthMulticast[20];
}ETH_SETTING;
ETH_SETTING eth_setting;

typedef struct{
    char strWlanIp[20];
    char strWlanMask[20];
    char strWlanGateway[20];
    char strWlanSSID[30];
    char strWlanPSK[100];
    unsigned char ucWlanEyp;
    unsigned char ucWlanDHCPSwitch;  
    unsigned char ucWlanEnable; 
}WLAN_SETTING;
WLAN_SETTING wlan_setting;

typedef struct{
    unsigned short usEncRate;
    unsigned char ucInterval;
    unsigned char ucFrameRate;
    unsigned char ucIQP;
    unsigned char ucPQP;
}ENCODER_SETTING;
ENCODER_SETTING encoder_setting;

typedef struct{
    unsigned char ucRTMPSwitch;    
    char strRTMPUrl[200];
    unsigned short usRTMPPort;
    unsigned char ucRTMPInterface;
}RTMP_SETTING;
RTMP_SETTING rtmp_setting;

typedef struct{
    unsigned char ucRTSPSwitch;  
    char strRTPBroadcastIp[20];
    char strRTSPUrl[100];
    unsigned short usRTPPort;
    unsigned short usRTSPPort;  
    unsigned char ucRTSPInterface;
}RTSP_SETTING;
RTSP_SETTING rtsp_setting;

//wang
typedef struct
{
	unsigned char ucIpAddress[128]; 
	unsigned char ucMultiAddress[128]; 
	unsigned int uuid[128]; 

} GROUP_PACK_S;
GROUP_PACK_S group_pack;

typedef struct
{
	char rxRename[128][40];
	char txRename[24][40];
	
} GROUP_RENAME_S;
GROUP_RENAME_S group_rename;

typedef struct
{
	char modeRename[10][20];
} MODE_RENAME_S;
MODE_RENAME_S mode_rename;
#endif

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


typedef struct
{
    unsigned int uuid;
    unsigned char heart_count;
    unsigned char is_hdmi_input;
    //char video_resolution;
    unsigned short video_width;
    unsigned short video_height;
    unsigned char video_framrate;
    unsigned char audio_type;
    unsigned char audio_sample;
    unsigned char audio_ch;
    char fw_version[20];
    char fw_status;
} TX_INFORMATION_S;


enum BAUD_RATE
{
    BAUD_9600   = 0,
    BAUD_115200 = 1
};
typedef enum BAUD_RATE BAUD_RATE_E;

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
    //unsigned int uuid;
    pNODE_S uuid;
    unsigned char osd_status;
    unsigned char tv_status;
    unsigned char video_source;
    unsigned char heart_count;
    unsigned char online_count;
    char fw_version[20];
    unsigned char fw_status;
    unsigned char data_type;
    CONTROL_DATA_S control_data;
} RX_INFORMATION_S;

typedef struct 
{
    unsigned char group_member[128];
    char group_alias[20];
} GROUP_S;

typedef struct
{
    unsigned char scene_source;
    unsigned char scene_member[20];
    char scene_alias[20];
} SCENE_S;

typedef struct
{
    GROUP_S group[20];
    SCENE_S scene[10];
    char TX_Alias[24][20];
    char RX_Alias[128][20];
} CONFIG_INFO_S;


#endif




