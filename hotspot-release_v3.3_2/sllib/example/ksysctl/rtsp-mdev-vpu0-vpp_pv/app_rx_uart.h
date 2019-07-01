#ifndef _APP_RX_UART_H
#define _APP_RX_UART_H

typedef struct osd_msg
{
	char osd_multicast[10];
	int x;
	int y;
	int page;
	int flag;
	struct osd_msg *next;
}OSD_MSG;

void *app_rx_uart_main(void);
void *serch_tx_multicast_main(void);
int find_multicast(char *mul);
void add_multicast(char *mul);
int del_multicast();
int clear_flag(void);
void init_sigcation();
void init_time();
void osd_menu_show();
void sort_multicast();

#endif
