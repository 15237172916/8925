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
#include <net/if.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>


static FILE *wpa_fp = NULL;
static FILE *dhcpc_fp = NULL;

int wifi_sta_start_wpa(void)
{
	char config[52];
	system("ifconfig wlan0 up");
#if 0
	system("/user/iwconfig wlan0 mode Managed &");

	sprintf(config, "/user/iwconfig wlan0 essid \"a-hotspot\" &");

	system(config);
#else
	//system("/user/iwconfig wlan0 mode Managed &");
	wpa_fp = popen("/user/wpa_supplicant -Dnl80211 -iwlan0 -dd -c/user/wpa_supplicant.conf&", "r");
	//wpa_fp = popen("/tmp/2016/hdmi/sllib/bin/wpa_supplicant -Dnl80211 -iwlan0 -c/tmp/2016/hdmi/sllib/bin/wpa_supplicant.conf-no-password&", "r");
	if(wpa_fp == NULL){
		printf("start wpa_supplicant error...\n");
		pclose(wpa_fp);
		wpa_fp = NULL;
		return -1;
	}
	//sleep(1);
#endif
	return 0;
}

int wifi_sta_stop_wpa(void)
{
	printf("%s\n",__func__);
#if 0
	system("ifconfig wlan0 down");
	return 0;
#endif
	if(wpa_fp > 0)
	{
		system("killall wpa_supplicant");
		pclose(wpa_fp);
		wpa_fp = NULL;
	}

	return 0;
}

int wifi_sta_start_udhcpc(void)
{
#if 0
	system("udhcpc -iwlan0&");
#else
	dhcpc_fp = popen("/user/udhcpc -iwlan0&", "r");
	if(dhcpc_fp == NULL){
		printf("start udhcpc error...\n");
		pclose(dhcpc_fp);
		dhcpc_fp = NULL;
		return -1;
	}
#endif
	return 0;
}

int wifi_sta_stop_udhcpc(void)
{
	//printf("%s\n",__func__);
	if(dhcpc_fp > 0)
	{
		system("killall udhcpc");
		pclose(dhcpc_fp);
		dhcpc_fp = NULL;
	}

	return 0;
}

int wifi_sta_get_ip_addr(char ip_addr[20])
{
	struct ifreq tmp;
	struct sockaddr_in *myaddr;
	int fd = 0;
	int ret = -1;

	strcpy(tmp.ifr_name, "wlan0");
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0){
		printf("create socket fail\n");
		return -1;
	}

	ret = ioctl(fd, SIOCGIFADDR, &tmp);
	if(ret < 0){
		printf("get ip socket ioctl error\n");
		close(fd);
		return -1;
	}

	myaddr = (struct sockaddr_in *)&(tmp.ifr_addr);
	strcpy(ip_addr, inet_ntoa(myaddr->sin_addr));

	close(fd);
	return 0;
}


