#ifndef		__rx_witch_multicast_H
#define     __rx_witch_multicast_H

#include <sl_types.h>





//multicast value	
char witch_multicast[][20] = {"239.255.42.01","239.255.42.02","239.255.42.03",\
"239.255.42.04","239.255.42.05","239.255.42.06","239.255.42.07","239.255.42.08","239.255.42.09",\
"239.255.42.10","239.255.42.11","239.255.42.12","239.255.42.13","239.255.42.14","239.255.42.15","239.255.42.16"};	
	


//jason add for witch_multicast
#define witch_multicast_1	4	//GPIO1_4       1	
#define witch_multicast_2	7	//GPIO1_7		2
#define witch_multicast_3	6	//GPIO1_6		3
#define witch_multicast_4	5	//GPIO1_5		4

#define GPIO_OUTPUT		1	//gpio mode is output
#define GPIO_INPUT		0	//gpio mode is input






void *tx_witch_multicast(void);







#endif





