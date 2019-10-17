
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <sl_gpio.h>

#include <sl_types.h>

#include "app_rx_io_ctl.h"



#define SYSFS_GPIO_DIR  "/sys/class/gpio"

extern SL_BOOL gbTestMode;


#include "sharemem.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern char multicast[20];
extern char web_flag;

//extern SHOWDATA g_ShowData;

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

#if 0 //20180809
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
#if 0
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

void Key_Init(void)
{
    //Key
	GPIO_openFd(KEY_IO);	//open gpio1_8 Fd
	GPIO_export(KEY_IO);	//export the gpio1_8 to users 
	GPIO_setDir(KEY_IO, GPIO_INPUT); //set the gpio1_8 direction is input 
}

/*895/896  After the program runs hotplug*/
void HDMI_HotPlug(void)
{
	printf("\r\n======HDMI_HotPlug======\r\n");
	GPIO_export(HDMI_Plug);
	GPIO_setDir(HDMI_Plug, GPIO_OUTPUT);
	GPIO_setValue(HDMI_Plug, GPIO_HIG_STA);
	usleep(500000);
	GPIO_setValue(HDMI_Plug, GPIO_LOW_STA);
}

SL_U32 get_key_value(void)
{
	SL_U32 value;
	
	GPIO_getValue(KEY_IO, &value);
	
	return value;
}
