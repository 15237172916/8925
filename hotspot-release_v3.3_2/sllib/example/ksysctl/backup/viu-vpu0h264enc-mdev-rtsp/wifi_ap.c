#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sl_os.h>

static FILE *ap_fp = NULL;
static FILE *dhcpd_fp = NULL;

int wifi_ap_set_static_ip(char *ip)
{
    char config[52];
    FILE *fp;
    
    sprintf(config, "ifconfig wlan0 %s netmask 255.255.255.0 up", ip);
    
    fp = popen(config, "r");
    if(fp == NULL){
        printf("set static ip error...\n");
        pclose(fp);
        return -1;
    }
    
    //sleep(1);
    pclose(fp);
    
    //system(config);
    
    return 0;
}

int wifi_ap_start_hostapd(void)
{
	//ap_fp = popen("/user/hostapd /user/rtl_hostapd_5G.conf&", "r");
	ap_fp = popen("/tmp/2016/hdmi/sllib/bin/hostapd-new /tmp/2016/hdmi/sllib/bin/rtl_hostapd_5G-no-password.conf -dd &", "r");
	if(ap_fp == NULL){
		printf("start hostapd error...\n");
		pclose(ap_fp);
		return -1;
	}
	//sleep(1);

	return 0;
}

int wifi_ap_start_udhcpd(void)
{
	dhcpd_fp = popen("/user/udhcpd /user/dhcpd.conf&", "r");
	if(dhcpd_fp == NULL){
		printf("start udhcpd error...\n");
		pclose(dhcpd_fp);
		return -1;
	}

	return 0;
}
