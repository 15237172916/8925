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
#define CONFIG_FILE ("/tmp/config.conf")


//#define CONFIG_FILE ("./config.conf")
#define NETWORK_INFO         ("Info1")
#define IP           ("IP")
#define SERVERIP           ("SERVERIP")

#define CFG_INFO ("[Info1]\n"\
			"IP=%s\n"\
			"SERVERIP=%s\n")
			
			


//CFG_INFO_S cfginfo = {{0},{0},0,0};
CFG_INFO_S cfginfo;



#if 1
static int get_random(void)
{
	struct timeval tv;
    gettimeofday(&tv,NULL);
	
	//printf("(unsigned int)time(NULL) : %d \n", (unsigned int)time(NULL));
	srand((unsigned int)(tv.tv_sec*1000000 + tv.tv_usec));
	
	return rand(); 
}
#endif


int AppInitCfgInfoDefault(void)
{
    //RUN_STATUS.ucHDMIStatus = HDMI_OUT;
    share_mem->sm_run_status.ucInputStatus = NONE;
    share_mem->sm_run_status.usWidth = 0;
    share_mem->sm_run_status.usHeight = 0;
    share_mem->sm_run_status.ucFrameRate = 0;
    share_mem->sm_run_status.usAudioSampleRate = 0;
    share_mem->sm_run_status.ucRTSPStatus = 0;
    share_mem->sm_run_status.ucRTPStatus = 0;
    share_mem->sm_run_status.ucRTMPStatus = 0;
    share_mem->sm_run_status.ucWiFiStatus = 0;   
#if 1
    share_mem->uuid = 0;
#endif
     
    strcpy(share_mem->sm_run_status.strHardwareVer, HD_VERSION);
    strcpy(share_mem->sm_run_status.strSoftwareVer, SW_VERSION);
    
    //ETH
    strcpy(share_mem->sm_eth_setting.strEthIp,"192.168.1.6");
    strcpy(share_mem->sm_eth_setting.strEthMask,"255.255.255.0");
    strcpy(share_mem->sm_eth_setting.strEthGateway,"192.168.1.3");        
    strcpy(share_mem->sm_eth_setting.strEthMulticast,"239.255.42.01");
    
    //WLAN
    share_mem->sm_wlan_setting.ucWlanDHCPSwitch = WLAN_DHCP_DISABLE;
    strcpy(share_mem->sm_wlan_setting.strWlanIp,"-");
    strcpy(share_mem->sm_wlan_setting.strWlanMask,"-");
    strcpy(share_mem->sm_wlan_setting.strWlanGateway,"-");     
    strcpy(share_mem->sm_wlan_setting.strWlanSSID,"a-hotspot");
    strcpy(share_mem->sm_wlan_setting.strWlanPSK,"");
    share_mem->sm_wlan_setting.ucWlanEyp=WPA;
    share_mem->sm_wlan_setting.ucWlanEnable=DISABLE; 
    
    //ENCODER
    share_mem->sm_encoder_setting.usEncRate = 1000;
    share_mem->sm_encoder_setting.ucInterval = 30;
    share_mem->sm_encoder_setting.ucIQP = 36;
    share_mem->sm_encoder_setting.ucPQP = 40;
    share_mem->sm_encoder_setting.ucFrameRate = 30;    
    
    //RTMP
    share_mem->sm_rtmp_setting.ucRTMPSwitch = RTMP_DISABLE;
    strcpy(share_mem->sm_rtmp_setting.strRTMPUrl,"");
    share_mem->sm_rtmp_setting.ucRTMPInterface = INTERFACE_ETH0;
    share_mem->sm_rtmp_setting.usRTMPPort = 1935;
    
    //RTSP
    share_mem->sm_rtsp_setting.ucRTSPSwitch = RTSP_DISABLE;
    share_mem->sm_rtsp_setting.usRTSPPort = 554;
    share_mem->sm_rtsp_setting.usRTPPort = 1234;
    strcpy(share_mem->sm_rtsp_setting.strRTPBroadcastIp, "239.255.42.42");
    share_mem->sm_rtsp_setting.ucRTSPInterface = INTERFACE_ETH0;
    //strcpy(share_mem->sm_rtsp_setting.strRTSPUrl, share_mem->sm_eth_setting.strEthIp);
    strcpy(share_mem->sm_rtsp_setting.strRTSPUrl, "main");
    //printf("init interval = %d  \n",  share_mem->sm_encoder_setting.ucInterval);

    return 1;
}

int AppInitCfgInfoFromFile(int *fp)
{
	int iRetCode = 0;
	char strTemp[20];

    printf("open config file \n");
	*fp = open(CONFIG_FILE, O_RDONLY);
	printf("fp = %d, *fp = %d\n",fp,*fp);
	if(*fp < 0)
	{
		share_mem->uuid = get_random();
		printf("%s open failed\n",CONFIG_FILE);
		return -1;
	}

    printf("open config file ok \n");
	if (cfginfo.flag)
		return 0;

	memset(&cfginfo,0,sizeof(cfginfo));

    printf("get eth ip \n");
    printf("share_mem->sm_eth_setting.strEthIp = %s \n", share_mem->sm_eth_setting.strEthIp);
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_IP",share_mem->sm_eth_setting.strEthIp);  
	
	printf("share_mem->sm_eth_setting.strEthIp = %s \n", share_mem->sm_eth_setting.strEthIp);
	if(iRetCode)  
	{  
		log_err("get Eth IP failed, %d !\n", iRetCode); 
		return -2;
	}
    printf("get eth ip ok\n");
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_MASK",share_mem->sm_eth_setting.strEthMask);  
	/*
	if(iRetCode)  
	{  
		log_err("get Eth Mask failed, %d !\n", iRetCode); 
		return -2;
	}
	*/
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_GATEWAY",share_mem->sm_eth_setting.strEthGateway);  
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_MULTICAST",share_mem->sm_eth_setting.strEthMulticast);  

	iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_IP",share_mem->sm_wlan_setting.strWlanIp);  
	iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_MASK",share_mem->sm_wlan_setting.strWlanMask);  
	iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_GATEWAY",share_mem->sm_wlan_setting.strWlanGateway);
	
	printf("%s %d\n",strTemp,iRetCode);
	iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_DHCP_SWITCH",strTemp);
	share_mem->sm_wlan_setting.ucWlanDHCPSwitch = atoi(strTemp);
	//printf("%s %d\n",strTemp,iRetCode);
	iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_SSID",share_mem->sm_wlan_setting.strWlanSSID);
	//printf("%s %d\n",strTemp,iRetCode);
    iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_PSK",share_mem->sm_wlan_setting.strWlanPSK);
	//printf("%s %d\n",strTemp,iRetCode);
	iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_ENCRYPTION",strTemp);
	share_mem->sm_wlan_setting.ucWlanEyp = atoi(strTemp);

	iRetCode = GetConfigStringValue(*fp,"ETH","WLAN_ENABLE",strTemp);
	share_mem->sm_wlan_setting.ucWlanEnable = atoi(strTemp);
	printf("WLAN_ENABLE %s %d\n",strTemp,iRetCode);	  
	  
	iRetCode = GetConfigStringValue(*fp,"ETH","ENC_RATE",strTemp);
	share_mem->sm_encoder_setting.usEncRate =  atoi(strTemp);  
	printf("Enc_rate %s %d\n",strTemp,share_mem->sm_encoder_setting.usEncRate);	
	
	iRetCode = GetConfigStringValue(*fp,"ETH","INTERVAL",strTemp);  
	//printf("%s %d\n",strTemp,iRetCode);
	share_mem->sm_encoder_setting.ucInterval = atoi(strTemp);
	printf("interval %s %d\n",strTemp,share_mem->sm_encoder_setting.ucInterval);	
		
	iRetCode = GetConfigStringValue(*fp,"ETH","IQP",strTemp);
	share_mem->sm_encoder_setting.ucIQP = atoi(strTemp);
	//printf("%s %d\n",strTemp,iRetCode);
	
    iRetCode = GetConfigStringValue(*fp,"ETH","PQP",strTemp);
	share_mem->sm_encoder_setting.ucPQP = atoi(strTemp);
	//printf("%s %d\n",strTemp,iRetCode);

    iRetCode = GetConfigStringValue(*fp,"ETH","FRAME_RATE",strTemp);
	share_mem->sm_encoder_setting.ucFrameRate = atoi(strTemp);
	printf("frame rate %s %d\n",strTemp,share_mem->sm_encoder_setting.ucFrameRate);
		
    iRetCode = GetConfigStringValue(*fp,"ETH","RTMP_SWITCH",strTemp);
	share_mem->sm_rtmp_setting.ucRTMPSwitch = atoi(strTemp);

    iRetCode = GetConfigStringValue(*fp,"ETH","RTMP_URL",share_mem->sm_rtmp_setting.strRTMPUrl);
    printf("url %s \n",share_mem->sm_rtmp_setting.strRTMPUrl);
    
    iRetCode = GetConfigStringValue(*fp,"ETH","RTMP_INTERFACE",strTemp);
	share_mem->sm_rtmp_setting.ucRTMPInterface = atoi(strTemp);
    
    iRetCode = GetConfigStringValue(*fp,"ETH","RTMP_PORT",strTemp);
	share_mem->sm_rtmp_setting.usRTMPPort = atoi(strTemp);

    iRetCode = GetConfigStringValue(*fp,"ETH","RTSP_SWITCH",strTemp);
	share_mem->sm_rtsp_setting.ucRTSPSwitch = atoi(strTemp); 
	
    iRetCode = GetConfigStringValue(*fp,"ETH","RTSP_PORT",strTemp);
	share_mem->sm_rtsp_setting.usRTSPPort = atoi(strTemp); 
	
    iRetCode = GetConfigStringValue(*fp,"ETH","RTP_PORT",strTemp);
	share_mem->sm_rtsp_setting.usRTPPort = atoi(strTemp); 

    iRetCode = GetConfigStringValue(*fp,"ETH","RTP_BROADCAST_IP",share_mem->sm_rtsp_setting.strRTPBroadcastIp);

    iRetCode = GetConfigStringValue(*fp,"ETH","RTSP_INTERFACE",strTemp);
	share_mem->sm_rtsp_setting.ucRTSPInterface = atoi(strTemp);

    iRetCode = GetConfigStringValue(*fp,"ETH","RTSP_URL",share_mem->sm_rtsp_setting.strRTSPUrl);


#if 1 //uuid
	iRetCode = GetConfigStringValue(*fp,"ETH","UUID",strTemp);
	share_mem->uuid = atoi(strTemp);
#endif
    
    /*
    if(INTERFACE_ETH0==share_mem->sm_rtsp_setting.ucRTSPInterface)
    {
        strcpy(share_mem->sm_rtsp_setting.strRTSPIp, share_mem->sm_eth_setting.strEthIp);
    }
    else
    {
        strcpy(share_mem->sm_rtsp_setting.strRTSPIp, share_mem->sm_wlan_setting.strWlanIp);
    }
    */
	
#if 0   
	printf("Info -----------------------------------\n");  
	printf("IP: %s\n", cfginfo.ip); 
	printf("SERVERIP: %s\n", cfginfo.serverip); 

	printf("-----------------------------------------------\n\n");
#endif
   // printf("file interval = %d  \n",  share_mem->sm_encoder_setting.ucInterval);
	return 0;
}

int AppWriteCfgInfotoFile(void)
{
	int iRetCode = 0; 
	char strTemp[20];
    FILE* fp;
    fp = fopen(CONFIG_FILE, "w");
    printf("write conf file \n");
    
    //Section Eth
    //fwrite("[ETH]\r\n",strlen("[ETH]\r\n"),1,fp);
    fprintf(fp,"[ETH]\n");
    fprintf(fp,"ETH_IP=%s\n",share_mem->sm_eth_setting.strEthIp);
    fprintf(fp,"ETH_MASK=%s\n",share_mem->sm_eth_setting.strEthMask);
    fprintf(fp,"ETH_GATEWAY=%s\n",share_mem->sm_eth_setting.strEthGateway);
    fprintf(fp,"ETH_MULTICAST=%s\n",share_mem->sm_eth_setting.strEthMulticast);
    
    //Section WLAN
    //fprintf(fp,"[WLAN]\n");
    fprintf(fp,"WLAN_DHCP_SWITCH=%d\n",share_mem->sm_wlan_setting.ucWlanDHCPSwitch);
    fprintf(fp,"WLAN_IP=%s\n"," - ");
    fprintf(fp,"WLAN_MASK=%s\n"," - ");
    fprintf(fp,"WLAN_GATEWAY=%s\n"," - ");
    fprintf(fp,"WLAN_SSID=%s\n",share_mem->sm_wlan_setting.strWlanSSID);
    fprintf(fp,"WLAN_PSK=%s\n",share_mem->sm_wlan_setting.strWlanPSK);
    fprintf(fp,"WLAN_ENCRYPTION=%d\n",share_mem->sm_wlan_setting.ucWlanEyp);
    fprintf(fp,"WLAN_ENABLE=%d\n",share_mem->sm_wlan_setting.ucWlanEnable);  
          
    //Section ENCODER
    //fprintf(fp,"[ENCODER]\n");
    fprintf(fp,"ENC_RATE=%d\n",share_mem->sm_encoder_setting.usEncRate);
    fprintf(fp,"INTERVAL=%d\n",share_mem->sm_encoder_setting.ucInterval);
    fprintf(fp,"IQP=%d\n",share_mem->sm_encoder_setting.ucIQP);
    fprintf(fp,"PQP=%d\n",share_mem->sm_encoder_setting.ucPQP);
    fprintf(fp,"FRAME_RATE=%d\n",share_mem->sm_encoder_setting.ucFrameRate);
    
    
    //Section RTMP
    //fprintf(fp,"[RTMP]\n");
    fprintf(fp,"RTMP_SWITCH=%d\n",share_mem->sm_rtmp_setting.ucRTMPSwitch);
    fprintf(fp,"RTMP_URL=%s\n",share_mem->sm_rtmp_setting.strRTMPUrl);
    fprintf(fp,"RTMP_INTERFACE=%d\n",share_mem->sm_rtmp_setting.ucRTMPInterface);
    fprintf(fp,"RTMP_PORT=%d\n",share_mem->sm_rtmp_setting.usRTMPPort);
            
    //Section RTSP
    //fprintf(fp,"[RTSP]\n");
    fprintf(fp,"RTSP_SWITCH=%d\n",share_mem->sm_rtsp_setting.ucRTSPSwitch);
    fprintf(fp,"RTSP_PORT=%d\n",share_mem->sm_rtsp_setting.usRTSPPort);
    fprintf(fp,"RTP_PORT=%d\n",share_mem->sm_rtsp_setting.usRTPPort);
    fprintf(fp,"RTP_BROADCAST_IP=%s\n",share_mem->sm_rtsp_setting.strRTPBroadcastIp);
    fprintf(fp,"RTSP_INTERFACE=%d\n",share_mem->sm_rtsp_setting.ucRTSPInterface);
    fprintf(fp,"RTSP_URL=%s\n",share_mem->sm_rtsp_setting.strRTSPUrl);
	
	//UUID
    fprintf(fp,"UUID=%d\n",share_mem->uuid);
    
    fprintf(fp,"[END]");
    
    fclose(fp);
    
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
	iRetCode = GetConfigStringValue(*fp,"ETH","ETH_MULTICAST",share_mem->sm_eth_setting.strEthMulticast);  

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
    fprintf(fp,"ETH_MULTICAST=%s\n",share_mem->sm_eth_setting.strEthMulticast);
    
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

