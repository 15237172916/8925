#ifndef _WIFI_STA_H
#define _WIFI_STA_H
int wifi_sta_start_wpa(void);
int wifi_sta_stop_wpa(void);
int wifi_sta_start_udhcpc(void);
int wifi_sta_stop_udhcpc(void);
int wifi_sta_get_ip_addr(char ip_addr[20]);
#endif
