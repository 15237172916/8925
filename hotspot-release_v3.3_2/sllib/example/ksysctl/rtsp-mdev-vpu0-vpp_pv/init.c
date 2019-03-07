#include "init.h"
#include <unistd.h>
#include <signal.h>

//static FILE *fpcmd = NULL;
#define MUL_ADDRESS		"route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0"
int init_eth(void)
{
    char syscmd[200];
    printf("init eth \n");
    strcpy(syscmd,"ifconfig eth0 ");
    strcat(syscmd,share_mem->sm_eth_setting.strEthIp);
    strcat(syscmd," netmask ");
    strcat(syscmd,share_mem->sm_eth_setting.strEthMask);
    system(syscmd);
    //multicast address configure
    system(MUL_ADDRESS);
    
    return 0;
}

int init_wlan(void)
{
    return 0;
}


