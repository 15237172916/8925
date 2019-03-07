#ifndef _WIFI_AP_H
#define _WIFI_AP_H
int wifi_ap_set_static_ip(char *ip);
int wifi_ap_start_hostapd(void);
int wifi_ap_start_udhcpd(void);
#endif
