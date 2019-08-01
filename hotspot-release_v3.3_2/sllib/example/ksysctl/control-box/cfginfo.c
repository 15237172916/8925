#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sl_debug.h>

#include "config.h"
#include "cfginfo.h"
#include "Setting.h"
#include "sharemem.h"

//#define CONFIG_FILE ("/user/configs/config.conf")

#define RM_CONFIG_FILE ("rm /tmp/configs/config.conf")
#define CONFIG_FILE1 ("/tmp/configs/modeName.conf")
#define CONFIG_FILE ("/tmp/configs/config.conf")
#define NETWORK_INFO         ("Info1")
#define IP           ("IP")
#define SERVERIP           ("SERVERIP")

#define CFG_INFO ("[Info1]\n"\
			"IP=%s\n"\
			"SERVERIP=%s\n")
			
			
//CFG_INFO_S cfginfo = {{0},{0},0,0};
CFG_INFO_S cfginfo;

int AppInitCfgInfoDefault(void)
{
	int i, j;
	char s[40];

	//TX information init
	for (i=0; i<24; i++)
	{
		strcpy(share_mem->config_info.TX_Alias[i], "TX");
		share_mem->tx_info[i].uuid = 0;
		share_mem->tx_info[i].audio_ch = 0;
		share_mem->tx_info[i].audio_sample = 0;
		share_mem->tx_info[i].audio_type = 0;
		share_mem->tx_info[i].video_framrate = 0;
		share_mem->tx_info[i].video_height = 0;
		share_mem->tx_info[i].video_width = 0;
		share_mem->tx_info[i].is_hdmi_input = 0;
		share_mem->tx_info[i].fw_status = 0;
		//share_mem->tx_info[i].fw_version = 1;
		strcpy(share_mem->tx_info[i].fw_version, "V0.0.1");	
	}

	//RX information init
	for (i=0; i<128; i++)
	{
		//sprintf(s,"");
		strcpy(share_mem->config_info.RX_Alias[i], "RX");
		share_mem->rx_info[i].uuid = 0;
		share_mem->rx_info[i].video_source = 1;
		share_mem->rx_info[i].heart_count = 0;
		share_mem->rx_info[i].data_type = 0;
		share_mem->rx_info[i].fw_status = 0;
		strcpy(share_mem->rx_info[i].fw_version, "V0.0.1");
		share_mem->rx_info[i].control_data.baud_rate = 0;
		share_mem->rx_info[i].control_data.data_bit = 0;
		share_mem->rx_info[i].control_data.data_format = 0;
		share_mem->rx_info[i].control_data.parity_bit = 0;
		for (j=0; j<128; j++)
		{
			share_mem->rx_info[i].control_data.off_data[j] = '0';
			share_mem->rx_info[i].control_data.on_data[j] = '0';	
		}
	}

	//Group init
	for (i=0; i<20; i++)
	{
		//share_mem->config_info.group[i].group_alias = 0;
		strcpy(share_mem->config_info.group[i].group_alias, "Group");
		for (j=0; j<128; j++)
		{
			share_mem->config_info.group[i].group_member[j] = 0;
		}		
	}
	
	//Scene
	for (i=0; i<10; i++)
	{
		share_mem->config_info.scene[i].scene_source = 1;
		strcpy(share_mem->config_info.scene[i].scene_alias, "Scene");
		for (j=0; j<20; j++)
		{
			share_mem->config_info.scene[i].scene_member[j] = 0;
		}
		
		//printf("scene_alias:%s \n", share_mem->config_info.scene[i].scene_alias);
	}

    return 1;
}

int AppWriteCfgInfotoFile(void)
{
	int iRetCode = 0 , i; 
	char strTemp[200];
    FILE* fp;
    fp = fopen(CONFIG_FILE, "w");
    printf("start write conf file \n");
	fprintf(fp, "[ETH]\n");

	//TX information
	for (i=0; i<24; i++)
	{
		fprintf(fp, "TX[%d]\n{\n", i+1);
		fprintf(fp, "\tTX[%d].ALIAS=%s\n", i+1, share_mem->config_info.TX_Alias[i]);
		fprintf(fp, "\tTX[%d].HDMI_INPUT=%d\n", i+1, share_mem->tx_info[i].is_hdmi_input);
		fprintf(fp, "\tTX[%d].AUDIO_TYPE=%d\n", i+1, share_mem->tx_info[i].audio_type);
		fprintf(fp, "\tTX[%d].AUDIO_SAMPLE=%d\n", i+1, share_mem->tx_info[i].audio_sample);
		fprintf(fp, "\tTX[%d].AUDIO_CH=%d\n", i+1, share_mem->tx_info[i].audio_ch);
		fprintf(fp, "\tTX[%d].VIDEO_FRAMRATE=%d\n", i+1, share_mem->tx_info[i].video_framrate);
		fprintf(fp, "\tTX[%d].VIDEO_WIDTH=%d\n", i+1, share_mem->tx_info[i].video_width);
		fprintf(fp, "\tTX[%d].VIDEO_HEIGHT=%d\n", i+1, share_mem->tx_info[i].video_height);
		fprintf(fp, "\tTX[%d].FW_STATUS=%d\n", i+1, share_mem->tx_info[i].fw_status);
    	fprintf(fp, "\tTX[%d].FW_VERSION=%s\n", i+1, share_mem->tx_info[i].fw_version);
		fprintf(fp, "}\n");
	}
	//RX information
	for (i=0; i<128; i++)
	{
		fprintf(fp, "RX[%d]\n{\n", i+1);
		fprintf(fp, "\tRX[%d].ALIAS=%s\n", i+1, share_mem->config_info.RX_Alias[i]);	
		fprintf(fp, "\tRX[%d].VIDEO_SOURCE=%d\n", i+1, share_mem->rx_info[i].video_source);
		fprintf(fp, "\tRX[%d].ONLINE_COUNT=%d\n", i+1, share_mem->rx_info[i].heart_count);
		fprintf(fp, "\tRX[%d].FW_STATUS=%d\n", i+1, share_mem->rx_info[i].fw_status);
		fprintf(fp, "\tRX[%d].FW_VWESION=%s\n", i+1, share_mem->rx_info[i].fw_version);
		fprintf(fp, "\tRX[%d].DATA_TYPE=%d\n", i+1, share_mem->rx_info[i].data_type);
		fprintf(fp, "\tRX[%d].BAUD_RATE=%d\n", i+1, share_mem->rx_info[i].control_data.baud_rate);
		fprintf(fp, "\tRX[%d].DATA_BIT=%d\n", i+1, share_mem->rx_info[i].control_data.data_bit);
		fprintf(fp, "\tRX[%d].DATA_FORMAT=%d\n", i+1, share_mem->rx_info[i].control_data.data_format);
		fprintf(fp, "\tRX[%d].PARITY_BIT=%d\n", i+1, share_mem->rx_info[i].control_data.parity_bit);
		fprintf(fp, "\tRX[%d].OFF_DATA=%s\n", i+1, share_mem->rx_info[i].control_data.off_data);
		fprintf(fp, "\tRX[%d].ON_DATA=%s\n", i+1, share_mem->rx_info[i].control_data.on_data);
		fprintf(fp, "}\n");
	}
	//Group 
	for (i=0; i<20; i++)
	{
		fprintf(fp, "GROUP[%d]\n{\n", i+1);
		fprintf(fp, "\tGROUP[%d].MEMBER=%s\n", i+1, share_mem->config_info.group[i].group_member);
		fprintf(fp, "\tGROUP[%d].ALIA=%s\n", i+1, share_mem->config_info.group[i].group_alias);
		fprintf(fp, "}\n");	
	}
	//Scene
	for (i=0; i<10; i++)
	{
		fprintf(fp, "SCENE[%d]\n{\n", i+1);
		fprintf(fp, "\tSCENE[%d].SOURCE=%d\n", i+1, share_mem->config_info.scene[i].scene_source);
		fprintf(fp, "\tSCENE[%d].MEMBER=%s\n", i+1, share_mem->config_info.scene[i].scene_member);
		fprintf(fp, "\tSCENE[%d].ALIAS=%s\n", i+1, share_mem->config_info.scene[i].scene_alias);
		//printf("SCENE[%d].ALIAS=%s\n", i+1, share_mem->config_info.scene[i].scene_alias);
		//fprintf(fp, "SCENE[%d].SOURCE=%d\n",i+1, share_mem->config_info.scene[i].scene_source);
		fprintf(fp, "}\n");
	}

    fprintf(fp,"[END]\n");
	fclose(fp);
}

int AppInitCfgInfoFromFile(int *fp)
{	
	int iRetCode = 0, i;
	char strTemp[20];
	char s[40];

    printf("open config file \n");
	*fp = open(CONFIG_FILE, O_RDONLY);
	printf("fp = %d, *fp = %d\n",fp,*fp);
	if(*fp < 0)
	{
		printf("%s open failed\n",CONFIG_FILE);
		return -1;
	}
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_GATEWAY",share_mem->sm_eth_setting.strEthGateway);

	memset(&cfginfo,0,sizeof(cfginfo));

    printf("get eth ip \n");

	
	return 0;
}
#if 0
int AppInitCfgInfoFromFile(int *fp)
{
	int iRetCode = 0; 
	char strTemp[20];

	*fp = open(CONFIG_FILE, O_RDWR);
	if(*fp < 0)
	{
		printf("%s open failed\n",CONFIG_FILE);
		return -1;
	}

	if (cfginfo.flag)
		return 0;

	memset(&cfginfo,0,sizeof(cfginfo));

	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_IP",share_mem->sm_eth_setting.strEthIp);  
	/*
	if(iRetCode)  
	{  
		log_err("get Eth IP failed, %d !\n", iRetCode); 
		return -2;
	}
	*/
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_MASK",share_mem->sm_eth_setting.strEthMask);  
	/*
	if(iRetCode)  
	{  
		log_err("get Eth Mask failed, %d !\n", iRetCode); 
		return -2;
	}
	*/
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_GATEWAY",share_mem->sm_eth_setting.strEthGateway);  

	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_IP",share_mem->sm_wlan_setting.strWlanIp);  
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_MASK",share_mem->sm_wlan_setting.strWlanMask);  
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_GATEWAY",share_mem->sm_wlan_setting.strWlanGateway);
	 printf("%s %d\n",strTemp,iRetCode);
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_DHCP_SWITCH",strTemp);
	share_mem->sm_wlan_setting.ucWlanDHCPSwitch = atoi(strTemp);
	printf("%s %d\n",strTemp,iRetCode);
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_SSID",share_mem->sm_wlan_setting.strWlanSSID);
printf("%s %d\n",strTemp,iRetCode);
    iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_PSK",share_mem->sm_wlan_setting.strWlanPSK);
    printf("%s %d\n",strTemp,iRetCode);
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_ENCRYPTION",strTemp);
	share_mem->sm_wlan_setting.ucWlanEyp = atoi(strTemp);
	printf("%s %d\n",strTemp,iRetCode);	  
	  
	iRetCode = GetConfigStringValue(*fp,"ENCODER","ENC_RATE",strTemp);
	share_mem->sm_encoder_setting.usEncRate =  atoi(strTemp);  
	printf("%s %d\n",strTemp,iRetCode);	
	iRetCode = GetConfigStringValue(*fp,"ENCODER","INTERVAL",strTemp);  

	printf("%s %d\n",strTemp,iRetCode);
	share_mem->sm_encoder_setting.ucInterval = atoi(strTemp);
	
	iRetCode = GetConfigStringValue(*fp,"ENCODER","IQP",strTemp);
	share_mem->sm_encoder_setting.ucIQP = atoi(strTemp);
	printf("%s %d\n",strTemp,iRetCode);
	
    iRetCode = GetConfigStringValue(*fp,"ENCODER","PQP",strTemp);
	share_mem->sm_encoder_setting.ucPQP = atoi(strTemp);
printf("%s %d\n",strTemp,iRetCode);

    iRetCode = GetConfigStringValue(*fp,"ENCODER","FRAME_RATE",strTemp);
	share_mem->sm_encoder_setting.ucFrameRate = atoi(strTemp);
	
    iRetCode = GetConfigStringValue(*fp,"RTMP","RTMP_SWITCH",strTemp);
	share_mem->sm_rtmp_setting.ucRTMPSwitch = atoi(strTemp);

    iRetCode = GetConfigStringValue(*fp,"RTMP","RTMP_URL",share_mem->sm_rtmp_setting.strRTMPUrl);
    
    iRetCode = GetConfigStringValue(*fp,"RTMP","RTMP_INTERFACE",strTemp);
	share_mem->sm_rtmp_setting.ucRTMPInterface = atoi(strTemp);
    
    iRetCode = GetConfigStringValue(*fp,"RTMP","RTMP_PORT",strTemp);
	share_mem->sm_rtmp_setting.usRTMPPort = atoi(strTemp);

    iRetCode = GetConfigStringValue(*fp,"RTSP","RTSP_SWITCH",strTemp);
	share_mem->sm_rtsp_setting.ucRTSPSwitch = atoi(strTemp); 
	
    iRetCode = GetConfigStringValue(*fp,"RTSP","RTSP_PORT",strTemp);
	share_mem->sm_rtsp_setting.usRTSPPort = atoi(strTemp); 
	
    iRetCode = GetConfigStringValue(*fp,"RTSP","RTP_PORT",strTemp);
	share_mem->sm_rtsp_setting.usRTPPort = atoi(strTemp); 

    iRetCode = GetConfigStringValue(*fp,"RTSP","RTP_BROADCAST_IP",share_mem->sm_rtsp_setting.strRTPBroadcastIp);

    iRetCode = GetConfigStringValue(*fp,"RTSP","RTSP_INTERFACE",strTemp);
	share_mem->sm_rtsp_setting.ucRTSPInterface = atoi(strTemp);

    //iRetCode = GetConfigStringValue(*fp,"RTSP","RTSP_IP",share_mem->sm_rtsp_setting.strRTSPIp);
    if(INTERFACE_ETH0==share_mem->sm_rtsp_setting.ucRTSPInterface)
    {
        strcpy(share_mem->sm_rtsp_setting.strRTSPIp, share_mem->sm_eth_setting.strEthIp);
    }
    else
    {
        strcpy(share_mem->sm_rtsp_setting.strRTSPIp, share_mem->sm_wlan_setting.strWlanIp);
    }
	
#if 0   
	printf("Info -----------------------------------\n");  
	printf("IP: %s\n", cfginfo.ip); 
	printf("SERVERIP: %s\n", cfginfo.serverip); 

	printf("-----------------------------------------------\n\n");
#endif
    printf("file interval = %d  \n",  share_mem->sm_encoder_setting.ucInterval);
	return 0;
}

int AppWriteCfgInfotoFile(void)
{
	int iRetCode = 0; 
	char strTemp[20];
    FILE* fp;    
    fp = fopen(CONFIG_FILE, "w");
    
    //Section Eth
    //fwrite("[ETH]\r\n",strlen("[ETH]\r\n"),1,fp);
    fprintf(fp,"[ETH]\n");
    fprintf(fp,"ETH_IP=%s\n",share_mem->sm_eth_setting.strEthIp);
    fprintf(fp,"ETH_MASK=%s\n",share_mem->sm_eth_setting.strEthMask);
    fprintf(fp,"ETH_GATEWAY=%s\n",share_mem->sm_eth_setting.strEthGateway);
    
    //Section WLAN
    fprintf(fp,"[WLAN]\n");
    fprintf(fp,"WLAN_DHCP_SWITCH=%d\n",share_mem->sm_wlan_setting.ucWlanDHCPSwitch);
    fprintf(fp,"WLAN_IP=%s\n",share_mem->sm_wlan_setting.strWlanIp);
    fprintf(fp,"WLAN_MASK=%s\n",share_mem->sm_wlan_setting.strWlanMask);
    fprintf(fp,"WLAN_GATEWAY=%s\n",share_mem->sm_wlan_setting.strWlanGateway);
    fprintf(fp,"WLAN_SSID=%s\n",share_mem->sm_wlan_setting.strWlanSSID);
    fprintf(fp,"WLAN_PSK=%s\n",share_mem->sm_wlan_setting.strWlanPSK);
    fprintf(fp,"WLAN_ENCRYPTION=%d\n",share_mem->sm_wlan_setting.ucWlanEyp);
        
    //Section ENCODER
    fprintf(fp,"[ENCODER]\n");
    fprintf(fp,"ENC_RATE=%d\n",share_mem->sm_encoder_setting.usEncRate);
    fprintf(fp,"INTERVAL=%d\n",share_mem->sm_encoder_setting.ucInterval);
    fprintf(fp,"IQP=%d\n",share_mem->sm_encoder_setting.ucIQP);
    fprintf(fp,"PQP=%d\n",encoder_setting.ucPQP);
    
    
    //Section RTMP
    fprintf(fp,"[RTMP]\n");
    fprintf(fp,"RTMP_SWITCH=%d\n",share_mem->sm_rtmp_setting.ucRTMPSwitch);
    fprintf(fp,"RTMP_URL=%s\n",share_mem->sm_rtmp_setting.strRTMPUrl);
    fprintf(fp,"RTMP_INTERFACE=%d\n",share_mem->sm_rtmp_setting.ucRTMPInterface);
    fprintf(fp,"RTMP_PORT=%d\n",share_mem->sm_rtmp_setting.usRTMPPort);
            
    //Section RTSP
    fprintf(fp,"[RTSP]\n");
    fprintf(fp,"RTSP_SWITCH=%d\n",share_mem->sm_rtsp_setting.ucRTSPSwitch);
    fprintf(fp,"RTSP_PORT=%d\n",share_mem->sm_rtsp_setting.usRTSPPort);
    fprintf(fp,"RTP_PORT=%d\n",share_mem->sm_rtsp_setting.usRTPPort);
    fprintf(fp,"RTP_BROADCAST_IP=%s\n",share_mem->sm_rtsp_setting.strRTPBroadcastIp);
     fprintf(fp,"RTSP_INTERFACE=%d\n",share_mem->sm_rtsp_setting.ucRTSPInterface);
    //fprintf(fp,"RTSP_IP=%s\n",share_mem->sm_rtsp_setting.strRTSPIp);
    
    fclose(fp);
    
}
#endif
#if 0
int AppInitCfgInfoDefault(void)
{
    //RUN_STATUS.ucHDMIStatus = HDMI_OUT;
    run_status.ucInputStatus = NONE;
    run_status.usWidth = 0;
    run_status.usHeight = 0;
    run_status.ucFrameRate = 0;
    run_status.usAudioSampleRate = 0;
    run_status.ucRTSPStatus = 0;
    run_status.ucRTPStatus = 0;
    run_status.ucRTMPStatus = 0;
    run_status.ucWiFiStatus = 0;    
    
    //ETH
    strcpy(eth_setting.strEthIp,"192.168.1.3");
    strcpy(eth_setting.strEthMask,"255.255.255.0");
    strcpy(eth_setting.strEthGateway,"192.168.1.1");        
    
    //WLAN
    wlan_setting.ucWlanDHCPSwitch = WLAN_DHCP_DISABLE;
    strcpy(wlan_setting.strWlanIp,"10.10.1.3");
    strcpy(wlan_setting.strWlanMask,"255.255.255.0");
    strcpy(wlan_setting.strWlanGateway,"10.10.1.1");     
    strcpy(wlan_setting.strWlanSSID,"");
    strcpy(wlan_setting.strWlanPSK,"");
    wlan_setting.ucWlanEyp=NO_EYP;
 
    
    //ENCODER
    encoder_setting.usEncRate = 1000;
    encoder_setting.ucInterval = 30;
    encoder_setting.ucIQP = 36;
    encoder_setting.ucPQP = 40;
    
    //RTMP
    rtmp_setting.ucRTMPSwitch = RTMP_DISABLE;
    strcpy(rtmp_setting.strRTMPUrl,"");
    rtmp_setting.ucRTMPInterface = INTERFACE_ETH0;
    
    //RTSP
    rtsp_setting.ucRTSPSwitch = RTSP_DISABLE;
    rtsp_setting.usRTSPPort = 554;
    rtsp_setting.usRTPPort = 1234;
    strcpy(rtsp_setting.strRTPBroadcastIp, "239.255.42.42");
    rtsp_setting.ucRTSPInterface = INTERFACE_ETH0;
    
    return 1;
}

int AppInitCfgInfoFromFile(int *fp)
{
	int iRetCode = 0; 
	char strTemp[40];

	*fp = open(CONFIG_FILE, O_RDWR);
	if(*fp < 0)
	{
		printf("%s open failed\n",CONFIG_FILE);
		return -1;
	}

	if (cfginfo.flag)
		return 0;

	memset(&cfginfo,0,sizeof(cfginfo));

	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_IP",eth_setting.strEthIp);  
	/*
	if(iRetCode)  
	{  
		log_err("get Eth IP failed, %d !\n", iRetCode); 
		return -2;
	}
	*/
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_MASK",eth_setting.strEthMask);  
	/*
	if(iRetCode)  
	{  
		log_err("get Eth Mask failed, %d !\n", iRetCode); 
		return -2;
	}
	*/
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_GATEWAY",eth_setting.strEthGateway);  

	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_IP",wlan_setting.strWlanIp);  
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_MASK",wlan_setting.strWlanMask);  
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_GATEWAY",wlan_setting.strWlanGateway);
	 
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_DHCP_SWITCH",strTemp);
	wlan_setting.ucWlanDHCPSwitch = atoi(strTemp);
	
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_SSID",wlan_setting.strWlanSSID);

    iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_PSK",wlan_setting.strWlanPSK);
    
	iRetCode = GetConfigStringValue(*fp,"WLAN","WLAN_ENCRYPTION",strTemp);
	wlan_setting.ucWlanEyp = atoi(strTemp);
	  
	  
	iRetCode = GetConfigStringValue(*fp,"ENCODER","ENC_RATE",strTemp);
	encoder_setting.usEncRate =  atoi(strTemp);  
	
	iRetCode = GetConfigStringValue(*fp,"ENCODER","INTERVAL",strTemp);  
	encoder_setting.ucInterval = atoi(strTemp);
	
	iRetCode = GetConfigStringValue(*fp,"ENCODER","IQP",strTemp);
	encoder_setting.ucIQP = atoi(strTemp);
	
    iRetCode = GetConfigStringValue(*fp,"ENCODER","PQP",strTemp);
	encoder_setting.ucPQP = atoi(strTemp);
	
    iRetCode = GetConfigStringValue(*fp,"RTMP","RTMP_SWITCH",strTemp);
	rtmp_setting.ucRTMPSwitch = atoi(strTemp);

    iRetCode = GetConfigStringValue(*fp,"RTMP","RTMP_URL",rtmp_setting.strRTMPUrl);
    
    iRetCode = GetConfigStringValue(*fp,"RTMP","RTMP_INTERFACE",strTemp);
	rtmp_setting.ucRTMPInterface = atoi(strTemp);
    

    iRetCode = GetConfigStringValue(*fp,"RTSP","RTSP_SWITCH",strTemp);
	rtsp_setting.ucRTSPSwitch = atoi(strTemp); 
	
    iRetCode = GetConfigStringValue(*fp,"RTSP","RTSP_PORT",strTemp);
	rtsp_setting.usRTSPPort = atoi(strTemp); 
	
    iRetCode = GetConfigStringValue(*fp,"RTSP","RTP_PORT",strTemp);
	rtsp_setting.usRTPPort = atoi(strTemp); 

    iRetCode = GetConfigStringValue(*fp,"RTSP","RTP_BROADCAST_IP",rtsp_setting.strRTPBroadcastIp);

    iRetCode = GetConfigStringValue(*fp,"RTSP","RTSP_INTERFACE",strTemp);
	rtsp_setting.ucRTSPInterface = atoi(strTemp);

	
#if 0   
	printf("Info -----------------------------------\n");  
	printf("IP: %s\n", cfginfo.ip); 
	printf("SERVERIP: %s\n", cfginfo.serverip); 

	printf("-----------------------------------------------\n\n");
#endif

	return 0;
}

int AppWriteCfgInfotoFile(void)
{
	int iRetCode = 0; 
	char strTemp[20];
    FILE* fp;    
    fp = fopen(CONFIG_FILE, "w");
    
    //Section Eth
    //fwrite("[ETH]\r\n",strlen("[ETH]\r\n"),1,fp);
    fprintf(fp,"[ETH]\n");
    fprintf(fp,"ETH_IP=%s\n",eth_setting.strEthIp);
    fprintf(fp,"ETH_MASK=%s\n",eth_setting.strEthMask);
    fprintf(fp,"ETH_GATEWAY=%s\n",eth_setting.strEthGateway);
    
    //Section WLAN
    fprintf(fp,"[WLAN]\n");
    fprintf(fp,"WLAN_DHCP_SWITCH=%d\n",wlan_setting.ucWlanDHCPSwitch);
    fprintf(fp,"WLAN_IP=%s\n",wlan_setting.strWlanIp);
    fprintf(fp,"WLAN_MASK=%s\n",wlan_setting.strWlanMask);
    fprintf(fp,"WLAN_GATEWAY=%s\n",wlan_setting.strWlanGateway);
    fprintf(fp,"WLAN_SSID=%s\n",wlan_setting.strWlanSSID);
    fprintf(fp,"WLAN_PSK=%s\n",wlan_setting.strWlanPSK);
    fprintf(fp,"WLAN_ENCRYPTION=%d\n",wlan_setting.ucWlanEyp);
        
    //Section ENCODER
    fprintf(fp,"[ENCODER]\n");
    fprintf(fp,"ENC_RATE=%d\n",encoder_setting.usEncRate);
    fprintf(fp,"INTERVAL=%d\n",encoder_setting.ucInterval);
    fprintf(fp,"IQP=%d\n",encoder_setting.ucIQP);
    fprintf(fp,"PQP=%d\n",encoder_setting.ucPQP);
    
    //Section RTMP
    fprintf(fp,"[RTMP]\n");
    fprintf(fp,"RTMP_SWITCH=%d\n",rtmp_setting.ucRTMPSwitch);
    fprintf(fp,"RTMP_URL=%s\n",rtmp_setting.strRTMPUrl);
    fprintf(fp,"RTMP_INTERFACE=%d\n",rtmp_setting.ucRTMPInterface);
            
    //Section RTSP
    fprintf(fp,"[RTSP]\n");
    fprintf(fp,"RTSP_SWITCH=%d\n",rtsp_setting.ucRTSPSwitch);
    fprintf(fp,"RTSP_PORT=%d\n",rtsp_setting.usRTSPPort);
    fprintf(fp,"RTP_PORT=%d\n",rtsp_setting.usRTPPort);
    fprintf(fp,"RTP_BROADCAST_IP=%s\n",rtsp_setting.strRTPBroadcastIp);
     fprintf(fp,"RTSP_INTERFACE=%d\n",rtsp_setting.ucRTSPInterface);
    
    fclose(fp);
    
}
#endif
/*
int InitCfgInfo(int *fp)
{
	int iRetCode = 0; 

	*fp = open(CONFIG_FILE, O_RDWR);
	if(*fp < 0)
	{
		printf("%s open failed\n",CONFIG_FILE);
		return -1;
	}

	if (cfginfo.flag)
		return 0;

	memset(&cfginfo,0,sizeof(cfginfo));

	iRetCode = GetConfigStringValue(*fp,NETWORK_INFO,IP,cfginfo.ip);  
	if(iRetCode)  
	{  
		log_err("get ip failed, %d !\n", iRetCode); 
		return -2;
	}
	iRetCode = GetConfigStringValue(*fp,NETWORK_INFO,SERVERIP,cfginfo.serverip);  
	if(iRetCode)  
	{  
		log_err("get serverip failed, %d !\n", iRetCode); 
		return -2;
	}

	cfginfo.flag = 1;

#if 1   
	printf("Info -----------------------------------\n");  
	printf("IP: %s\n", cfginfo.ip); 
	printf("SERVERIP: %s\n", cfginfo.serverip); 

	printf("-----------------------------------------------\n\n");
#endif

	return 0;
}
*/

int update_cfg_info(int fpConfig, CFG_INFO_S *pcfginfo)
{
	char cfginfobuff[512] = {0};

	if(NULL == pcfginfo)
		return -1;

	lseek(fpConfig, 0 ,SEEK_SET); 

	sprintf(cfginfobuff, CFG_INFO, 
			pcfginfo->ip, 
			pcfginfo->serverip);

	//printf("%s\n",cfginfobuff);

	write(fpConfig,cfginfobuff, strlen(cfginfobuff));
	sync();
	close(fpConfig);
	return 0;
}

CFG_INFO_S *GetCfgInfo()
{
    return &cfginfo;
}
