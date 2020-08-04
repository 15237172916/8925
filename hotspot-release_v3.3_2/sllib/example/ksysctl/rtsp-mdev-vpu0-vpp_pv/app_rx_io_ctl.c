#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sl_gpio.h>
#include <sl_types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../version.h"
#include "app_rx_io_ctl.h"
#include "sharemem.h"
#include "digit_led.h"


#define SYSFS_GPIO_DIR  "/sys/class/gpio"

char g_key_display = 0;
unsigned char shumaduan[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};  //0~f duanxuanxinhao

/**
 * GPIO_export() - Export GPIO to user space
 *
 * @param Gpio: GPIO Number
 *
 * @return 0: OK
 *         !0: ERROR
 */
SL_S32 GPIO_export(SL_U32 Gpio)
{
    int fd, len;
    char buf[GPIO_MAXBUF];
    //printf("GPIO is %d\n", Gpio);  //
    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if (fd < 0) {
        log_err("gpio/export");
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", Gpio);
    write(fd, buf, len);
    close(fd);

    return 0;
}

/**
 * GPIO_unexport() - Unexport GPIO
 *
 * @param Gpio: GPIO Number
 *
 * @return 0: OK
 *        !0: ERROR
 */
SL_S32 GPIO_unexport(SL_U32 Gpio)
{
    int fd, len;
    char buf[GPIO_MAXBUF];

    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fd < 0) {
        log_err("gpio/export");
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", Gpio);
    write(fd, buf, len);
    close(fd);
    return 0;
}

/**
 * GPIO_setDir() - Configure GPIO for input or output
 *
 * @param Gpio: GPIO Number
 * @param OutFlag: 1 output; 0 input
 *
 * @return 0: OK
 *         !0: ERROR
 */
SL_S32 GPIO_setDir(SL_U32 Gpio, SL_U32 OutFlag)
{
    int fd, len;
    char buf[GPIO_MAXBUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", Gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        log_err("gpio/direction");
        return fd;
    }

    if (OutFlag)
        write(fd, "out", 4);
    else
        write(fd, "in", 3);

    close(fd);
    return 0;
}

/**
 * GPIO_setValue() - Write supporting values of 0 and 1
 *
 * @param Gpio: GPIO Number
 * @param Value: 0 or 1
 *
 * @return 0: OK
 *         !0: ERROR
 */
SL_S32 GPIO_setValue(SL_U32 Gpio, SL_U32 Value)
{
    int fd, len;
    char buf[GPIO_MAXBUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", Gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        log_err("gpio/set-value");
        return fd;
    }

    if (Value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

/**
 * GPIO_getValue() - Read supporting values of 0 and 1
 *
 * @param Gpio: GPIO Number
 * @param *Value: 0 or 1
 *
 * @return 0: OK
 *         !0: ERROR
 */
SL_S32 GPIO_getValue(SL_U32 Gpio, SL_U32 *Value)   //SL_U32 *Value
{
    int fd, len;
    char buf[GPIO_MAXBUF];
    char ch;
	//extern int t;

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", Gpio);

    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        log_err("gpio/get-value");
        return fd;
    }

    read(fd, &ch, 1);

    if (ch != '0') {
        *Value = 1;
	//t = 1; ////
    } else {
        *Value = 0;
	//t = 1;
    }
	//return t;   
    close(fd);
    return 0;
}

/**
 * GPIO_setEdge() - Configure GPIO an interrupt source
 *
 * @param Gpio: GPIO Number
 * @param *Edge: "rising", "falling" and "both"
 *
 * @return 0: OK
 *         !0: ERROR
 */
SL_S32 GPIO_setEdge(SL_U32 Gpio, char *Edge)
{
    int fd, len;
    char buf[GPIO_MAXBUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", Gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        log_err("gpio/set-edge");
        return fd;
    }

    write(fd, Edge, strlen(Edge) + 1);
    close(fd);
    return 0;
}

/**
 * GPIO_openFd() - GPIO fd open
 *
 * @param Gpio: GPIO Number
 *
 * @return 0: OK
 *         !0: ERROR
 */
SL_S32 GPIO_openFd(SL_U32 Gpio)
{
    int fd, len;
    char buf[GPIO_MAXBUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", Gpio);

    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
        log_err("gpio/fd_open");
    }
    return fd;
}

/**
 * GPIO_closeFd() - GPIO fd close
 *
 * @param fd
 *
 * @return 0: OK
 *         !0: ERROR
 */
SL_S32 GPIO_closeFd(SL_S32 Fd)
{
    return close(Fd);
}

void Sleep(int ms)
{  
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000; // 20 ms
    select(0, NULL, NULL, NULL, &delay);
}

#if 0
void SetLightMode(unsigned char uLightNo, unsigned char uMode, unsigned char uCnt)
{
	g_LightMode[uLightNo].uMode = uMode; 
	g_LightMode[uLightNo].uCnt = uCnt;
}

void *app_rx_light_ctl_main(void)
{
    unsigned int i=0;
    
    g_LightMode[0].uLightNo = 4; //mid	GPIO1-04   
    g_LightMode[1].uLightNo = 5; //high GPIO1-05
    g_LightMode[2].uLightNo = 6; //low	GPIO1-06  
    g_LightMode[3].uLightNo = 7; //HDMI GPIO1-07   
    
    for(i=0;i<=3;i++)
    {
        g_LightMode[i].uMode = 1; //off init
        g_LightMode[i].uCnt = 3;
        g_LightMode[i].uCStatus = 1;
    }
    
    printf("================Light Test=================\n");
    sleep(1);
                
    while(1)
    {
		//i++;
        usleep(500000);
        for(i=0;i<=3;i++)
        {
            if(0==g_LightMode[i].uMode || 1 == g_LightMode[i].uMode) //always on or off
            {
                if(g_LightMode[i].uCStatus != g_LightMode[i].uMode) //status changed
                {
                    printf("Led %d Status change to %d\n",g_LightMode[i].uLightNo,g_LightMode[i].uMode);
                    g_LightMode[i].uCStatus = g_LightMode[i].uMode;
                    GPIO_setValue(g_LightMode[i].uLightNo, g_LightMode[i].uCStatus);                    
                }
            }
            else if(2==g_LightMode[i].uMode)//flash
            {
                if(g_LightMode[i].uCnt<g_LightMode[i].uInterval)
                {
                    g_LightMode[i].uCnt++;
                }
                else
                {
					//printf("Led %d flash\n",g_LightMode[i].uLightNo);
                    g_LightMode[i].uCnt = 0;
                    if(0==g_LightMode[i].uCStatus)
                    {
                        g_LightMode[i].uCStatus = 1;
                    }
                    else
                    {
                        g_LightMode[i].uCStatus = 0;
                    }
                    
                    GPIO_setValue(g_LightMode[i].uLightNo, g_LightMode[i].uCStatus);                    
                }
            }
        }
    }
}
#endif
#if 0
void  *app_rx_io_ctl_main(void)
{
  	int i;
  	
  	//SL_U32 sum;
	SL_U32 Value;
	
	
#if 0 //LED
	GPIO_openFd(LED_SIG_LOW);
	GPIO_export(LED_SIG_LOW);	   			
	GPIO_setDir(LED_SIG_LOW, GPIO_OUTPUT);
    GPIO_setValue(LED_SIG_LOW, GPIO_HIG_STA);
	
	GPIO_openFd(LED_SIG_MOD);
	GPIO_export(LED_SIG_MOD);	   			
	GPIO_setDir(LED_SIG_MOD, GPIO_OUTPUT);
	GPIO_setValue(LED_SIG_MOD, GPIO_HIG_STA);

	GPIO_openFd(LED_SIG_HIG);
	GPIO_export(LED_SIG_HIG);	   			
	GPIO_setDir(LED_SIG_HIG, GPIO_OUTPUT);
	GPIO_setValue(LED_SIG_HIG, GPIO_HIG_STA);

	GPIO_openFd(LED_HDMI);
	GPIO_export(LED_HDMI);
	GPIO_setDir(LED_HDMI, GPIO_OUTPUT); //output
	GPIO_setValue(LED_HDMI, GPIO_HIG_STA); //high
	
	GPIO_openFd(LED_SYS_STA);
	GPIO_export(LED_SYS_STA);	
	GPIO_setDir(LED_SYS_STA, GPIO_OUTPUT); //output
	GPIO_setValue(LED_SYS_STA, GPIO_HIG_STA); //high
#endif
	//while(1) sleep(1);
	
	//Key
	GPIO_openFd(KEY_IO);	//open gpio1_8 Fd
	GPIO_export(KEY_IO);	//export the gpio1_8 to users 
	GPIO_setDir(KEY_IO, GPIO_INPUT); //set the gpio1_8 direction is input 
	
    while (1) //sleep(1);
    {
	    int flag=0;
	    int time=0;
	    #if 1
	    if(0 == get_key_value())
	    {
			Sleep(5);
			
			if(0 == get_key_value())
			{
				flag = 1;
				do
				{
					//Value = get_key_value();		  
					time = time+1;
					Sleep(1);
					
					if(time > 1500)
					{
						flag = 3;
						
						reboot1(); //reset
					}
					//printf("time = %d \n", time);
				}while (0 == get_key_value());
				/*
				if(time > 1500)
				{
					flag = 3;
					
					reboot1(); //reset
				}
				*/
				for(i = 1;i<500;i++)
				{
					Sleep(1);
					Value = get_key_value();
					if(Value ==0)
					{
						Sleep(10);
						Value = get_key_value();
						if(Value == 0)
						{
							flag=2;
						}
					}
				}
				printf("FALG is %d\n", flag);	
			}
	    }
	    #endif
	    #if 1
        switch(flag)
        {
           
           case 1:  //  Change bitrate
	       {
/*
                 if(SL_TRUE == gbTestMode)
		        {
		            gbTestMode = SL_FALSE;
		            g_ShowData.bShowFlag = SL_TRUE;
		            g_ShowData.cText ="Test Mode On - change bitrate";
                    printf("gbTestMode is FALSE\n");
                    GPIO_setValue(32, 0);
                           
		        }
		        else
		        {
		            gbTestMode = SL_TRUE;
		            g_ShowData.bShowFlag = SL_TRUE;
		            g_ShowData.cText ="Test Mode Off - change bitrate";		    
                            printf("gbTestMode is TRUE\n");
                    GPIO_setValue(32, 0);
		        }
*/
#if 0
                if(SL_TRUE==gbTestMode)
                {
					g_ShowData.bShowFlag = SL_TRUE;
					g_ShowData.cText ="Change bitrate";		    
					
                    
                    if(SL_FALSE == g_RemoteCmd.bSendFlag)
                    {
                        g_RemoteCmd.bSendFlag = SL_TRUE;
                        g_RemoteCmd.uCmdBuf[0]=1; //change bitrate
                        printf("Send Remote Command case 1\n");            
                    }
                }
#endif

#if 0 //20180809 change video level
				printf("*********************flag = %d*****************\n ", flag);
				g_ShowData.bShowFlag = SL_TRUE;
		        g_ShowData.cText ="Change Video Level";
		        if (SL_FALSE == g_RemoteCmd.bSendFlag)
				{
					g_RemoteCmd.bSendFlag = SL_TRUE;
					g_RemoteCmd.uCmdBuf[0]=1; //change bitrate
					printf("Send Remote Command case 1\n");            
				}
#endif
#if 0	//change mulitcast 
				printf("*********************flag = %d*****************\n ", flag);
				unsigned int add, add1;
				char str[20] = {0};
				
				inet_pton(AF_INET, share_mem->sm_eth_setting.strEthMulticast, &add);
				
				add = ntohl(add); //host
				printf("add = 0x%x \n", add);
				add+=1;
				add1 = add & 0xFF;
				iic_display_test(add1); //
				printf("add1 = %d \n", add1);
				
				add = htonl(add); //net

				printf("add = 0x%x \n", add);
				inet_ntop(AF_INET, &add, str, sizeof(str));
				memcpy(share_mem->sm_eth_setting.strEthMulticast, str, sizeof(str));
				printf(str);
				printf("\n");
				share_mem->ucUpdateFlag = 1;
#endif			

#if 0	//display IP address in the screen
				//process_osd_text_solid(10, 10, share_mem->sm_eth_setting.strEthIp);


#endif
		        break;
	       }
		
           case 3:  //reboot
           {
                reboot1();
                break;
           }
           case 2://Data Test Enable/Disable
           {
                if(SL_FALSE == gbTestMode)
		        {
	                gbTestMode = SL_TRUE;
                    g_ShowData.bShowFlag = SL_TRUE;
		            g_ShowData.cText ="Data Test On";
					
                    if(SL_FALSE == g_RemoteCmd.bSendFlag)
                    {
                        printf("g_RemoteCmd.uCmdBuf[0]= %d before set\n", g_RemoteCmd.uCmdBuf[0]);
                        g_RemoteCmd.bSendFlag = SL_TRUE;
                        g_RemoteCmd.uCmdBuf[0]=3; //Data Test 
                        g_RemoteCmd.uCmdBuf[1]=1; //On
                        
                        printf("Send Remote Command %d after setting\n",g_RemoteCmd.uCmdBuf[0]);            
                    }
                }
                else
		        {
		            gbTestMode = SL_FALSE;
		            g_ShowData.bShowFlag = SL_TRUE;

		            g_ShowData.cText ="Data Test Off";		

                    if(SL_FALSE == g_RemoteCmd.bSendFlag)
                    {
                        printf("g_RemoteCmd.uCmdBuf[0]= %d before set\n", g_RemoteCmd.uCmdBuf[0]);                    
                        g_RemoteCmd.bSendFlag = SL_TRUE;
                        g_RemoteCmd.uCmdBuf[0]=3; //Data Test 
                        g_RemoteCmd.uCmdBuf[1]=0; //Off
                        
                        printf("Send Remote Command %d after setting\n",g_RemoteCmd.uCmdBuf[0]);   
                    }
		        }
		
                break;
			}
        }
        #endif
		Sleep(100);
    }  

}
#endif

void *app_rx_io_ctl_main(void)
{
    SL_U32 value1,value2;
    char str[20]={0};
    int flag1=0;
    int flag2=0;
    int cnt=0;
    int i;
    unsigned int mul_add, ip_add, tmp_add;

	//get ip address
	inet_pton(AF_INET, share_mem->sm_eth_setting.strEthIp, &ip_add);
	ip_add = ntohl(ip_add); //host
	ip_add &= 0xFF;
	//get multicast address
	inet_pton(AF_INET, share_mem->sm_eth_setting.strEthMulticast, &mul_add);
	mul_add = ntohl(mul_add); //host
	mul_add &= 0xFF;
	printf("ip_add: %d, mul_add: %d \n", ip_add, mul_add);
#ifdef HSV7011
	flag1=ip_add/10; //shi
	flag2=ip_add%10; //ge
#endif
#ifdef HSV7012
	flag1=mul_add/10; //shi
	flag2=mul_add%10; //ge
#endif
    printf("flag1:%d,flag2:%d\n",flag1,flag2);

	GPIO_openFd(KEY_ONE);	//open gpio1_5 Fd
	GPIO_export(KEY_ONE);	//export the gpio1_5 to users 
	GPIO_setDir(KEY_ONE, GPIO_INPUT); //set the gpio1_5 direction is input 

	GPIO_openFd(KEY_TWO);	//open gpio1_6 Fd
	GPIO_export(KEY_TWO);	//export the gpio1_6 to users 
	GPIO_setDir(KEY_TWO, GPIO_INPUT); //set the gpio1_6 direction is input 

    sleep(1);
    digit_led_writebyte(0x48,0x11);
    digit_led_writebyte(0x68, 0xff);
    digit_led_writebyte(0x6A, 0xff);
    sleep(1);
    digit_led_writebyte(0x68, shumaduan[flag1]);
    digit_led_writebyte(0x6A, shumaduan[flag2]);
    
#if 1
	while(1)
	{
		GPIO_getValue(KEY_ONE, &value1);
		GPIO_getValue(KEY_TWO, &value2);
		//printf("flag1:%d, flag2:%d \n",flag1, flag2);
		if (0==value1) //key 1
		{
			usleep(400000);    //消抖加延时
			if (0==value1)
			{
				flag1++;

				if ((0 == flag2) && (flag1 > 12)) //num > 120
				{
					flag1 = 1;
				}
				if ((flag2 > 8) && (flag1 > 12)) //num > 128
				{
					flag1 = 1;
				}
				if (flag1 > 12)
				{
					flag1 = 0;
				}
				printf("flag1:%d\n",flag1); 
				digit_led_writebyte(0x48,0x11);
				digit_led_writebyte(0x68, shumaduan[flag1]);
				cnt=0;
			}
        }
        if (0==value2)
        {
            usleep(400000);  //消抖加延时
            if(0==value2)
            {
				flag2++;
				printf("flag2:%d\n",flag2);
				#if 1
				if ((12 == flag1) && (flag2 > 8))// address>128
				{
					flag2 = 0;
				}
				if ((0 == flag1) && (flag2 > 9))//address = 01-09
				{
					flag2 = 1; //01
				}
				if (flag2 > 9) //
				{
					flag2 = 0;
				}
				#endif
                digit_led_writebyte(0x48,0x11);
                digit_led_writebyte(0x6A,shumaduan[flag2]);
                cnt=0;
            }
        }
		tmp_add = (flag1*10)+flag2;
		usleep(100000);    //100ms
		cnt++;
		if(cnt>30)    //3s
		{
#ifdef HSV7011
			if (ip_add!=tmp_add)
#endif
#ifdef HSV7012
			if (mul_add!=tmp_add)
#endif
			{
#ifdef HSV7011
				ip_add = tmp_add;
				sprintf(str, "192.168.1.%d", tmp_add);
				strcpy(share_mem->sm_eth_setting.strEthIp, str);
				printf("str :%s\n",share_mem->sm_eth_setting.strEthIp);
				AppWriteCfgInfotoFile();
				init_eth(); //ip configer
#endif

#ifdef HSV7012
				mul_add = tmp_add;
				//printf("mul_add : %d \n", mul_add);
				sprintf(str,"239.255.42.%d",tmp_add);
				//printf("str :%s\n",str);
				strcpy(share_mem->sm_eth_setting.strEthMulticast, str);
				printf("str :%s\n",share_mem->sm_eth_setting.strEthMulticast);
				share_mem->ucUpdateFlag = 1;
#endif
                for(i=0;i<3;i++)
                {
                    usleep(600000);
                    digit_led_writebyte(0x6A, 0xff);
                    digit_led_writebyte(0x68, 0xff);
                    usleep(300000);
                    digit_led_writebyte(0x6A, shumaduan[flag2]);
                    digit_led_writebyte(0x68, shumaduan[flag1]);
                }
                cnt=0;
            }
        }
        
    }//while end
#endif
}
#if 0
void signal_light_all_off(void)
{
	SetLightMode(HIGH_LED, LED_ON, 0);
	SetLightMode(MID_LED, LED_ON, 0);
	SetLightMode(LOW_LED, LED_ON, 0);
}

void signal_light_all_on(void)
{
	SetLightMode(HIGH_LED, LED_OFF, 3);
	SetLightMode(MID_LED, LED_OFF, 0);
	SetLightMode(LOW_LED, LED_OFF, 0);
}

void signal_light_high_level(void)
{
	SetLightMode(HIGH_LED, LED_ON, 3);
	SetLightMode(MID_LED, LED_ON, 0);
	SetLightMode(LOW_LED, LED_ON, 0);
}

void signal_light_mid_level(void)
{
	SetLightMode(HIGH_LED, LED_OFF, 0);
	SetLightMode(MID_LED, LED_ON, 3);
	SetLightMode(LOW_LED, LED_ON, 0);
}

void signal_light_low_level(void)
{			
	SetLightMode(HIGH_LED, LED_OFF, 0);
	SetLightMode(MID_LED, LED_OFF, 3);
	SetLightMode(LOW_LED, LED_ON, 0);
}

void signal_light_flash(void)
{
	signal_light_all_on();
	SetLightMode(HIGH_LED, LED_FLASH, 3);
    SetLightMode(MID_LED, LED_FLASH, 3);
    SetLightMode(LOW_LED, LED_FLASH, 3);
}

void HDMI_light_off(void)
{
	SetLightMode(HDMI_LED, LED_OFF, 0);
}

void HDMI_light_on(void)
{
	SetLightMode(HDMI_LED, LED_ON, 0);
}

void test_light(char value)
{
	GPIO_openFd(4);
	GPIO_export(4);	   			
	GPIO_setDir(4, 1);
    GPIO_setValue(4, value);
}
#endif
SL_U32 get_key_value(void)
{
	SL_U32 value;
	
	GPIO_getValue(KEY_IO, &value);
	
	return value;
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
	GPIO_openFd(OSD_MULTICAST);
	GPIO_export(OSD_MULTICAST);
	GPIO_setDir(OSD_MULTICAST, GPIO_INPUT);
#endif

	printf("-----------IP switch-----------\n");
	while (1)
	{
		sleep(1);
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
		//printf("tmp2 = 0x%x \n", tmp2);

		#if 1
		if (tmp2 != tmp1)
		{
			tmp2 = tmp1;
			sprintf(str, "192.168.1.%d", tmp1+1);
			printf(str);
			if (strcmp(str, share_mem->sm_eth_setting.strEthIp))
			{
				strcpy(share_mem->sm_eth_setting.strEthIp, str);
				printf(share_mem->sm_eth_setting.strEthIp);
				AppWriteCfgInfotoFile();
				init_eth(); //ip configer
			}
		}
		#endif

		GPIO_getValue(OSD_MULTICAST, &key);

		if (!key)
		{
			usleep(300000);
			key_count++;
			printf("key_count : %d \n", key_count);
			if (!key)
			{
				g_key_display = 1;
				printf("\n\nKey OSD Multicast display \n\n");
				#if 0
				if (0==state)
				{
					printf("\n\n OSD Multicast display \n\n");
					g_key_display = 1;
					state = 1;
					//save_264file_flag = 1;
				}
				else
				{
					printf("\n\n OSD Multicast disable \n\n");
					state = 0;
					g_key_display = 0;
					//save_264file_flag = 0;
				}
				#endif
			}
			if (key_count > 10)
			{
				printf("reboot \n");
				reboot1();
			}
		}
		else
		{
			key_count = 0;
		}

		usleep(20000);
	}
}

void HPD_Init()
{
	GPIO_openFd(HDMI_HPD_CONTROL);
	GPIO_export(HDMI_HPD_CONTROL);
	GPIO_setDir(HDMI_HPD_CONTROL, GPIO_OUTPUT);
	GPIO_setValue(HDMI_HPD_CONTROL, GPIO_LOW_STA);
}

void Reset_HPD(void)
{
	GPIO_setValue(HDMI_HPD_CONTROL, GPIO_HIG_STA);
	sleep(1);
	GPIO_setValue(HDMI_HPD_CONTROL, GPIO_LOW_STA);
	sleep(1);
}

void OFF_HPD(void)
{
	GPIO_setValue(HDMI_HPD_CONTROL, GPIO_HIG_STA);
}

void ON_HPD(void)
{
	GPIO_setValue(HDMI_HPD_CONTROL, GPIO_LOW_STA);
}


