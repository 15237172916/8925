#ifndef _SETTING_H_
#define _SETTING_H_


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

#define WPA 2
#define WEP 1
#define NO_EYP 0

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
    char strSoftwareVer[20]
}RUN_STATUS;

RUN_STATUS run_status;

typedef struct{
    char strEthIp[20];
    char strEthMask[20];
    char strEthGateway[20];
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
#endif
