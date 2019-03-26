#include "rx_witch_multicast.h"
#include <sl_gpio.h>
#include "sharemem.h"
#include <string.h>







 /*func: It's config multicast by witch
 *author: Jason chen, 2018/8/30
 *return: success is 0, error is -1;
 */
void *rx_witch_multicast(void)
{
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
			share_mem->ucUpdateFlag = 1;
			tmp2 = tmp;
			puts(share_mem->sm_eth_setting.strEthIp);
		}
		//printf("tmp is %d\n", tmp);
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




void *IP_switch(void)
{
	SL_U32 value, tmp1, tmp2, key, key_count = 0;
	char state = 0;
	
	char str[20] = {0};
	char str_tmp[100] = {0};
	
#if 1 //
	GPIO_openFd(IP_SWITCH_1);
	GPIO_export(IP_SWITCH_1);	   			
	GPIO_setDir(IP_SWITCH_1, GPIO_INPUT);
   

	GPIO_openFd(IP_SWITCH_2);
	GPIO_export(IP_SWITCH_2);	   			
	GPIO_setDir(IP_SWITCH_2, GPIO_INPUT);
	

	GPIO_openFd(IP_SWITCH_3);
	GPIO_export(IP_SWITCH_3);	   			
	GPIO_setDir(IP_SWITCH_3, GPIO_INPUT);


	GPIO_openFd(IP_SWITCH_4);
	GPIO_export(IP_SWITCH_4);
	GPIO_setDir(IP_SWITCH_4, GPIO_INPUT);
	
	
	GPIO_openFd(IP_SWITCH_5);
	GPIO_export(IP_SWITCH_5);	
	GPIO_setDir(IP_SWITCH_5, GPIO_INPUT);
#if 1	
	GPIO_openFd(IP_SWITCH_6);
	GPIO_export(IP_SWITCH_6);
	GPIO_setDir(IP_SWITCH_6, GPIO_INPUT);
	
	
	GPIO_openFd(IP_SWITCH_7);
	GPIO_export(IP_SWITCH_7);	
	GPIO_setDir(IP_SWITCH_7, GPIO_INPUT);
#endif

#endif
	
	
	printf("-----------IP switch-----------\n");
	while (1)
	{
		tmp1 = 0x00;
		
		GPIO_getValue(IP_SWITCH_1, &value); //1
		tmp1 |= value; //0x01
		tmp1 = tmp1 << 1; //0x02
		GPIO_getValue(IP_SWITCH_2, &value); //1
		tmp1 = tmp1 | value; //0x03
		tmp1 = tmp1 << 1;
		GPIO_getValue(IP_SWITCH_3, &value);
		tmp1 |= value;
		tmp1 = tmp1 << 1;
		GPIO_getValue(IP_SWITCH_4, &value);
		tmp1 |= value;
		tmp1 = tmp1 << 1;
		GPIO_getValue(IP_SWITCH_5, &value);
		tmp1 |= value;
		tmp1 = tmp1 << 1;
		GPIO_getValue(IP_SWITCH_6, &value);
		tmp1 |= value;
		tmp1 = tmp1 << 1;
		GPIO_getValue(IP_SWITCH_7, &value);
		tmp1 |= value;
		
		//printf("tmp1 = 0x%x \n", tmp1);
		#if 1
		if (tmp2 != tmp1)
		{
			tmp2 = tmp1;
			sprintf(str, "192.168.1.%d", tmp1+1);
			strcpy(share_mem->sm_eth_setting.strEthIp, str);
			puts(share_mem->sm_eth_setting.strEthIp);
			AppWriteCfgInfotoFile();
			init_eth(); //ip configer
		}
		#endif
		
		usleep(20000);
	}
}


















