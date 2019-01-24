#include "init.h"
#include <unistd.h>
#include <signal.h>

//static FILE *fpcmd = NULL;
#define MUL_ADDRESS		"route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0" 
#define DEFAULT_ROUTE 	"route add default gw 192.168.1.1"
int init_eth(void)
{
    char syscmd[200];
    strcpy(syscmd,"ifconfig eth0 ");
    strcat(syscmd,share_mem->sm_eth_setting.strEthIp);
    printf(syscmd);
    printf("share_mem->sm_eth_setting.strEthIp=%s\n",share_mem->sm_eth_setting.strEthIp);
    strcat(syscmd," netmask ");
    strcat(syscmd,share_mem->sm_eth_setting.strEthMask);
    system(syscmd);
    printf("ifconfig eth0=%s\n",syscmd);
    
    //multicast address configure
    system(MUL_ADDRESS);
    system(DEFAULT_ROUTE);
    
    return 0;
}
/*int init_eth(void)
{
    char syscmd[200];
    strcpy(syscmd,"ifconfig eth0 ");
   // strcat(syscmd,share_mem->sm_eth_setting.strEthIp);
   strcat(syscmd,share_mem->sm_rtsp_setting.strRTPBroadcastIp);
    strcat(syscmd," netmask ");
    strcat(syscmd,share_mem->sm_eth_setting.strEthMask);
    system(syscmd);
    printf("ifconfig eth0=%s\n",syscmd);
    return 0;
    
}*/
int init_wlan(void)
{
    return 0;
}


