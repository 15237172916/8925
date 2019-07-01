#ifndef _APP_TX_UART_H
#define _APP_TX_UART_H

typedef struct check_rx
{
	int packageIP[1];
	int flag;
	struct check_rx *next;
}CHECK_RX;


void *app_tx_uart_main(void);
void *send_tx_multicast_main(void);

void add_packip(SL_U8 packageIP[1]);
int find_packip(SL_U8 packageIP[1]);
int clear_flag(void);
int del_packip();
void init_sigcation();
void init_time();

#endif
