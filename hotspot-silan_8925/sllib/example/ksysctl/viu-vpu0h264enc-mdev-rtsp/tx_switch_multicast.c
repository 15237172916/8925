#include "tx_switch_multicast.h"
#include <sl_gpio.h>
#include "sharemem.h"
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sl_types.h>
#include <arpa/inet.h>
#include <unistd.h>



static pthread_t  Udp_multicast_handler;

/*******Jason add 20190115 for udp multicast*****/
#define Udp_Multicast_PORT	10001

typedef struct Udp_Multicast
{
	unsigned long	header  :32;	
	unsigned char	channel :8;
}Udp_Multicast_t;
static int mark;

static int tx_Udp_multicast_main(void)
{
	int lens = 0, ret = -1;
	const int opt = -1;
	char * s = malloc(50*sizeof(char));
	unsigned long check_code = 0xABCDE;
	Udp_Multicast_t udp_Multicast;
	
	//socket
    int sock_client;
    struct sockaddr_in servaddr;
    socklen_t servlen_addr_length;
    
    servlen_addr_length = sizeof(servaddr);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(Udp_Multicast_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("0.0.0.0"); 
    //servaddr.sin_addr.s_addr = inet_addr(serverip);
    //servaddr.sin_addr.s_addr = inet_addr("192.168.1.6");   
	
ReSocket:
	sock_client = socket(AF_INET,SOCK_DGRAM, 0); //UDP
	if (sock_client < 0)
    {
        printf("uart Create Socket Failed!\n");
        perror("socket");
        sleep(1);
        close(sock_client);
        goto ReSocket;
    }
    //printf("jason Udp_Multicast test sock_client = %d\n", sock_client);

#if 1   
	//set socket 
	ret = setsockopt(sock_client,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt));

	if (ret = bind(sock_client, (struct sockaddr_in *) &servaddr, sizeof(servaddr)) < 0) //bind
	{
		perror("bind");
		printf("server broadcast bind error \n");
		sleep(1);
		close(sock_client);
		goto ReSocket;
	}
	
	printf("bind secceed \n");
    struct timeval timeout;

    timeout.tv_sec = 5;                 
    timeout.tv_usec = 0;  
    setsockopt(sock_client,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
    setsockopt(sock_client,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
#endif
	
	while(1)
	{
		usleep(500);		
		//printf("rx_Udp_multicast_main start +++++++++++++++ \n");
		lens = recvfrom(sock_client, &udp_Multicast, sizeof(udp_Multicast), 0, (struct sockaddr_in *)&servaddr, &servlen_addr_length);
		if(lens<0)
		{
			perror("recvfrom");
			printf("**********************rx_Udp_multicast_main revfrom failed ****************8\n");
			sleep(1);
			//close(sock_client);
			//goto ReSocket;
		}
		if(lens>0)
		{
			//printf("jason test recvfrom ----------------OK \n");	
			//printf("channel is %d\n", udp_Multicast.channel);   	//jason test
			//printf("header is 0x%x\n", udp_Multicast.header);					
			if (check_code == udp_Multicast.header)
			{	
				if (0==mark)
				{
					if(0<udp_Multicast.channel && udp_Multicast.channel<=16)
					{
						//printf("udp_Multicast.channel %d \n", udp_Multicast.channel);
						sprintf(s, "239.255.42.%d", udp_Multicast.channel);
						printf(s);
						
						strcpy(share_mem->sm_eth_setting.strEthMulticast, s);
						share_mem->ucUpdateFlag = 1;
						//printf("udp_Multicast set  ----------------OK \n");
					}
					else
					{
						printf("channel error, must be 1-16\n");
					}
				}
			}
			
		}					
	}
	free(s);
	close(sock_client);	
	
	return 0;
}



 /*func: It's config multicast by witch
 *author: Jason chen, 2018/8/30
 *return: success is 0, error is else;
 */	
void *tx_switch_multicast(void)
{
	static int tmp1;
	static int tmp2;
	
	int tmp;
	SL_U32 value = 0;
	int i,j,k,m;
	int ret, val;
	int pthread_flag = 1;
	
	//GPIO_INIT
	GPIO_openFd(switch_multicast_1);   //gpio_4
	GPIO_export(switch_multicast_1);	   			
	GPIO_setDir(switch_multicast_1, GPIO_INPUT);
   
	GPIO_openFd(switch_multicast_2);   //gpio_5
	GPIO_export(switch_multicast_2);	   			
	GPIO_setDir(switch_multicast_2, GPIO_INPUT);

	GPIO_openFd(switch_multicast_3);   //gpio_6
	GPIO_export(switch_multicast_3);	   			
	GPIO_setDir(switch_multicast_3, GPIO_INPUT);
   
	GPIO_openFd(switch_multicast_4);   //gpio_7
	GPIO_export(switch_multicast_4);	   			
	GPIO_setDir(switch_multicast_4, GPIO_INPUT);
  
	tmp2 = 18;   //frist time change init 
	
#if 0	
	// pthread_create
	pthread_flag = pthread_create(&Udp_multicast_handler, NULL, tx_Udp_multicast_main, NULL);		
	printf("tx_Udp_multicast_main test start : [main]\n"); 
#endif
		
	while(1)
	{
#if 1		
		i=0; j=0; k=0; m=0;
		GPIO_getValue(switch_multicast_1, &value);
		if(!value)
		{
			i = 1;
			value = 1;
		}		
		GPIO_getValue(switch_multicast_2, &value);
		if(!value)
		{
			j = 1;
			value = 1;	
		}		
		GPIO_getValue(switch_multicast_3, &value);
		if(!value)
		{
			k = 1;
			value = 1;		
		}		
		GPIO_getValue(switch_multicast_4, &value);
		if(!value)
		{		
			m = 1;
			value = 1;
		}
		tmp = i*8 + j*4 + k*2 + m;
		//mark = tmp;	
					 
		if(tmp2 != tmp)
		{

			strcpy(share_mem->sm_eth_setting.strEthMulticast, switch_multicast+tmp);
			share_mem->ucUpdateFlag = 1;
			tmp2 = tmp;
			puts(share_mem->sm_eth_setting.strEthMulticast);

		}
		//printf("tmp is %d_______________\n", tmp);
		sleep(1);	
#endif

#if 0	
			tmp = 0x00;
			GPIO_getValue(switch_multicast_1, &value);	
			tmp |= value;	
			tmp = tmp << 1;

			printf("tmp1 is 0x%x\n", tmp);
			printf("\n  value1 is %d    \n", value);
			GPIO_getValue(switch_multicast_2, &value);
			tmp |= value;	
			tmp = tmp << 1;
			
			printf("tmp2!! is 0x%x\n", tmp);	
			printf("\n  value2 is %d    \n", value);	
			GPIO_getValue(switch_multicast_3, &value);		
			tmp |= value;	
			tmp = tmp << 1;		
			
			printf("tmp3 is 0x%x\n", tmp);	
			printf("\n  value3 is %d    \n", value);	
			GPIO_getValue(switch_multicast_4, &value);		
			tmp |= value;		
			
			printf("tmp4 is 0x%x\n", tmp);
			printf("\n  value4 is %d    \n", value);	
			
			mark = tmp;	
			printf("\n mark is 0x%x-----\n", mark);
			if(tmp2 != tmp)		
			{			
				if(0!=mark)
				{
					strcpy(share_mem->sm_eth_setting.strEthMulticast, switch_multicast+tmp);
					share_mem->ucUpdateFlag = 1;
					tmp2 = tmp;
					puts(share_mem->sm_eth_setting.strEthMulticast);	
					printf("tmp is %d_______________\n", tmp);			
				}
				//printf("tmp is %d_______________\n", tmp);
				
			}
			sleep(1);
#endif
	}
	return 0;
}



















