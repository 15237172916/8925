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
void *rx_witch_multicast(void);




//IP switch IO 
#define IP_SWITCH_7		30 //GPIO1_30
#define IP_SWITCH_6		5 //GPIO1_05
#define IP_SWITCH_5		6 //GPIO1_06
#define IP_SWITCH_4		7 //GPIO1_07
#define IP_SWITCH_3		4 //GPIO1_04
#define IP_SWITCH_2		36 //GPIO2_04
#define IP_SWITCH_1		34 //GPIO2_02
void *IP_switch(void);




#define GPIO_OUTPUT		1	//gpio mode is output
#define GPIO_INPUT		0	//gpio mode is input





#endif





