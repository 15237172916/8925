#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Setting.h"
#include "sharemem.h"

int InitShareMem(void)
{
	int shmid;

	shmid = shmget((key_t)1234, sizeof(SHARE_MEM), 0666|IPC_CREAT);	
	if(shmid == -1)  
	{  
		printf("shmget failed\n");  
		exit(0);  
	}  

	share_mem = shmat(shmid, (void*)0, 0);  
	if(share_mem == NULL)  
	{
		printf("shmat failed\n");
		exit(0);  
	}
	printf("Memory attached at %X\n", (int)share_mem);  

    //share_mem->sm_eth_setting = eth_setting;
}

void main(void)
{
    char str[2048];
    char strTemp[100];
    char strTempC[100];

    InitShareMem();
	
    strcpy(str,"<set>");
    strcat(str,"<rtmp_type>");
    sprintf(strTemp,"%d",share_mem->sm_rtmp_setting.ucRTMPInterface);
    strcat(str,strTemp);
    strcat(str,"</rtmp_type>");

    strcat(str,"<rtsp_type>");
    sprintf(strTemp,"%d",share_mem->sm_rtsp_setting.ucRTSPInterface);
    strcat(str,strTemp);
    strcat(str,"</rtsp_type>");

    strcat(str,"<ip>");
    strcat(str,share_mem->sm_eth_setting.strEthIp);
    strcat(str,"</ip>");

    strcat(str,"<mask>");
    strcat(str,share_mem->sm_eth_setting.strEthMask);
    strcat(str,"</mask>");

    strcat(str,"<gateway>");
    strcat(str,share_mem->sm_eth_setting.strEthGateway);
    strcat(str,"</gateway>");

    strcat(str,"<wifi_status>");
    sprintf(strTemp,"%d",share_mem->sm_run_status.ucWiFiStatus);
    strcat(str,strTemp);
    strcat(str,"</wifi_status>");
    
    strcat(str,"<wifi_dhcp>");
    sprintf(strTemp, "%d",share_mem->sm_wlan_setting.ucWlanDHCPSwitch);
    strcat(str,strTemp);    
    strcat(str,"</wifi_dhcp>");
    
    strcat(str,"<wifi_ip>");
    strcat(str,share_mem->sm_wlan_setting.strWlanIp);
    strcat(str,"</wifi_ip>");

    strcat(str,"<wifi_mask>");
    strcat(str,share_mem->sm_wlan_setting.strWlanMask);
    strcat(str,"</wifi_mask>");

    strcat(str,"<wifi_gateway>");
    strcat(str,share_mem->sm_wlan_setting.strWlanGateway);
    strcat(str,"</wifi_gateway>");

    strcat(str,"<wifi_essid>");
    strcat(str,share_mem->sm_wlan_setting.strWlanSSID);
    strcat(str,"</wifi_essid>");

    strcat(str,"<wifi_psk>");
    strcat(str,share_mem->sm_wlan_setting.strWlanPSK);
    strcat(str,"</wifi_psk>");
        
    strcat(str,"<wifi_type>");
    sprintf(strTemp, "%d",share_mem->sm_wlan_setting.ucWlanEyp);
    strcat(str,strTemp);    
    strcat(str,"</wifi_type>");

    strcat(str,"<wifi_enable>");
    sprintf(strTemp, "%d",share_mem->sm_wlan_setting.ucWlanEnable);
    strcat(str,strTemp);    
    strcat(str,"</wifi_enable>");

    strcat(str,"</set>");
    
     printf("content-Type:text/xml\n\n");
       printf("%s\n",str);
       
//   printf("<set><ip>192.168.100.30</ip><mask>255.255.255.0</mask><gateway>192.168.0.1</gateway><dns0>192.168.0.1</dns0><dns1>192.168.0.1</dns1><mac>16:20:05:00:10:62</mac><dhcp>0</dhcp><net_type>1</net_type><wifi_exist>1</wifi_exist><wifi_ip>192.168.0.32</wifi_ip><wifi_netmask>255.255.255.0</wifi_netmask><wifi_essid></wifi_essid><wifi_psk></wifi_psk><wifi_gateway>192.168.0.1</wifi_gateway><wifi_type>0</wifi_type><wifi_status>0</wifi_status></set>\n");
    
}

