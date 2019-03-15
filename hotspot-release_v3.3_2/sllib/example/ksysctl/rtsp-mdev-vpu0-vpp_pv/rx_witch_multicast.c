#include "rx_witch_multicast.h"
#include <sl_gpio.h>
#include "sharemem.h"
#include <string.h>
#include "app_rx_io_ctl.h"




extern char multicast[20];
extern char switch_flag;

 /*func: It's config multicast by witch
 *author: Jason chen, 2018/8/30
 *return: success is 0, error is -1;
 */
void *rx_witch_multicast(void)
{
#if 0
	static int tmp2;
	int tmp;
	SL_U32 value;
	int i,j,k,m;
	
	//GPIO_INIT
	GPIO_openFd(witch_multicast_1);   //gpio_4
	GPIO_export(witch_multicast_1);	   			
	GPIO_setDir(witch_multicast_1, GPIO_INPUT);
   
	GPIO_openFd(witch_multicast_2);   //gpio_5
	GPIO_export(witch_multicast_2);	   			
	GPIO_setDir(witch_multicast_2, GPIO_INPUT);

	GPIO_openFd(witch_multicast_3);   //gpio_6
	GPIO_export(witch_multicast_3);	   			
	GPIO_setDir(witch_multicast_3, GPIO_INPUT);
   
	GPIO_openFd(witch_multicast_4);   //gpio_7
	GPIO_export(witch_multicast_4);	   			
	GPIO_setDir(witch_multicast_4, GPIO_INPUT);
  
	tmp2 = 18;   //frist time change init 


	while(1)
	{
		i=0; j=0; k=0; m=0;
		GPIO_getValue(witch_multicast_1, &value);
		if(!value)
		{
			i = 1;
			value = 1;
		}		
		GPIO_getValue(witch_multicast_2, &value);
		if(!value)
		{
			j = 1;
			value = 1;	
		}		
		GPIO_getValue(witch_multicast_3, &value);
		if(!value)
		{
			k = 1;
			value = 1;		
		}		
		GPIO_getValue(witch_multicast_4, &value);
		if(!value)
		{		
			m = 1;
			value = 1;
		}
		tmp = i*8 + j*4 + k*2 + m;
				
		if(tmp2 != tmp)
		{
			strcpy(share_mem->sm_eth_setting.strEthMulticast, witch_multicast+tmp);
			share_mem->ucUpdateFlag = 1;
			tmp2 = tmp;
			puts(share_mem->sm_eth_setting.strEthMulticast);
		}
		//printf("tmp is %d\n", tmp);
		sleep(1);



	}
#endif
	int tmp=1;
	GPIO_openFd(KEY_IO);  
	GPIO_export(KEY_IO);	   			
	GPIO_setDir(KEY_IO, GPIO_INPUT);
	

	printf("enter switch while\n");
	while(1)
	{
		static int key_flag=1;
		if(key_flag && (get_key_value()==0))
		{
			usleep(1000);
			key_flag=0;
			if(get_key_value()==0)
			{
				tmp=!tmp;
				strcpy(multicast, witch_multicast+tmp);
				switch_flag=1;
			}
		}
		else if(get_key_value()==1)
			key_flag=1;
			
		sleep(1);
	}
	
	
	
	
	
	
	
	
	
		/*sencond not useflu now*/
		/*
			tmp = 0x00;
			GPIO_getValue(witch_multicast_1, &value);	
			tmp |= value;	
			tmp = tmp << 1;

			printf("tmp1 is 0x%x\n", tmp);
			GPIO_getValue(witch_multicast_2, &value);
			tmp |= value;	
			tmp = tmp << 1;
			printf("tmp2!! is 0x%x\n", tmp);		
			GPIO_getValue(witch_multicast_3, &value);		
			tmp |= value;	
			tmp = tmp << 1;		
			printf("tmp3 is 0x%x\n", tmp);	
			GPIO_getValue(witch_multicast_4, &value);		
			tmp |= value;		
			tmp = tmp << 1;
			printf("tmp4 is 0x%x\n", tmp);

			if(tmp2 != tmp)		
			{			
				printf("web start \n");
				strcpy(share_mem->sm_eth_setting.strEthMulticast, witch_multicast+tmp);
				tmp2 = tmp;
				puts(share_mem->sm_eth_setting.strEthMulticast);
			}
			//printf("tmp2 is 0x%x\n", tmp2);
			*/
	return 0;
}



















