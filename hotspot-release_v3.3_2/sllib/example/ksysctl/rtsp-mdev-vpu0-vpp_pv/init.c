#include "init.h"
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//static FILE *fpcmd = NULL;

#define CONFIG_IP_FILE ("/tmp/configs/ip.conf")
#define SH 	"#! /bin/sh \n"

#define MUL_ADDRESS		"route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0"
#define DEFAULT_ROUTE 	"route add default gw 192.168.1.1"


int init_eth(void)
{
    char syscmd[200];
    FILE *fp = -1;
    int ret = -1;
    
    printf("init eth start \n");
    strcpy(syscmd,"ifconfig eth0 ");
    strcat(syscmd,share_mem->sm_eth_setting.strEthIp);
    strcat(syscmd," netmask ");
    strcat(syscmd,share_mem->sm_eth_setting.strEthMask);
	
    printf(syscmd);
    system(syscmd);

#if 0 //create ip configure file then set ip address
    fp = fopen(CONFIG_IP_FILE, "w");
    if(fp == NULL)
	{
		printf("%s open failed\n",CONFIG_IP_FILE);
		return -1;
	}
	printf("open %s is ok\n", CONFIG_IP_FILE);
	fprintf(fp, SH);
	fprintf(fp, syscmd);
	/*
    ret = write(fp, SH, strlen(SH));
    if (ret<0)
    {
		printf("write %s error \n", SH);
	}
    write(fp, syscmd, strlen(syscmd));
    {
		printf("write %s error \n", syscmd);
	}
	*/
    fclose(fp);
#endif
	
    //multicast route address configure
    system(MUL_ADDRESS);
    
    system(DEFAULT_ROUTE);
    printf("init eth finish \n");

    return 0;
}

void set_eth0_ip(void)
{
    static FILE *pf = NULL;
    char syscmd[100] = {0};
    strcpy(syscmd,"ifconfig eth0 ");
    //strcat(syscmd,share_mem->sm_eth_setting.strEthIp);
    strcat(syscmd,share_mem->sm_eth_setting.strEthIp);
    printf(syscmd);
    //pf = popen(syscmd, "r");
    pf = popen("ifconfig eth0 192.168.1.12", "r");
    if (NULL == pf)
    {
        printf("\n------set eth0 ip error----- \n");
    }
    pclose(pf);
}

int init_wlan(void)
{
    return 0;
}


