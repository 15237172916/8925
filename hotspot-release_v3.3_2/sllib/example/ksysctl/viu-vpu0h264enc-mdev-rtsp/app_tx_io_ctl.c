#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sl_gpio.h>
#include <sl_types.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>                         // 包含AF_INET相关结构

#include "../version.h"
#include "app_tx_io_ctl.h"
#include "cfginfo.h" 
#include "sharemem.h"
#include "digit_led.h"


#define SYSFS_GPIO_DIR  "/sys/class/gpio"

extern SL_BOOL gbTestMode;

unsigned char shumaduan[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};  //0~f duanxuanxinhao
//extern SHOWDATA g_ShowData;
extern int reboot1(void);

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
 *         !0: ERROR
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
    //return t;   ///
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

void *app_tx_io_ctl_main(void)
{
    SL_U32 value1,value2;
    char str[20]={0};
    char tx_multicast_ip[20];
    char *p;
    int flag1=0;
    int flag2=0;
    int cnt=0;
    int i,num;
    static int ip_add, mul_add, tmp_add;

    //get ip address
	inet_pton(AF_INET, share_mem->sm_eth_setting.strEthIp, &ip_add);
	ip_add = ntohl(ip_add); //host
	ip_add &= 0xFF;
	//get multicast address
	inet_pton(AF_INET, share_mem->sm_eth_setting.strEthMulticast, &mul_add);
	mul_add = ntohl(mul_add); //host
	mul_add &= 0xFF;
	printf("ip_add: %d, mul_add: %d \n", ip_add, mul_add);

	flag1=mul_add/10; //shi
	flag2=mul_add%10; //ge
    tmp_add = mul_add;
    GPIO_openFd(KEY_ONE);   //open gpio1_5 Fd
    GPIO_export(KEY_ONE);   //export the gpio1_5 to users 
    GPIO_setDir(KEY_ONE, GPIO_INPUT); //set the gpio1_5 direction is input 

    GPIO_openFd(KEY_TWO);   //open gpio1_6 Fd
    GPIO_export(KEY_TWO);   //export the gpio1_6 to users 
    GPIO_setDir(KEY_TWO, GPIO_INPUT); //set the gpio1_6 direction is input 
    
    sleep(1);
    digit_led_writebyte(0x48, 0x11);
    digit_led_writebyte(0x68, 0xff);
    digit_led_writebyte(0x6A, 0xff);

    sleep(1);
    digit_led_writebyte(0x68, shumaduan[flag1]);
    digit_led_writebyte(0x6A, shumaduan[flag2]);

#if 1
    while(1)
    {
        GPIO_getValue(KEY_ONE, &value1); //tens
        GPIO_getValue(KEY_TWO, &value2); //unit
        if (0==value1)  //tens
        {
            usleep(400000);
            if (0==value1)
            {
                flag1++;
                printf("flag1 : %d \n", flag1);
#ifdef HSV7011 //ip = 1-24
                if ((flag2 > 4) && (flag1 >= 2)) // num > 24
                {
                    flag1 = 0;
                }
                if ((0 == flag2) && (flag1 > 2)) //01 - 09
                {
                    flag1 = 1;
                }
                if (flag1 > 2)
                {
                    flag1 = 0;
                }
#endif
#ifdef HSV7012 //ip = 1-128
                if ((0 == flag2) && (flag1 > 12)) //1 - 9
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
#endif
                digit_led_writebyte(0x48, 0x11);
                digit_led_writebyte(0x68, shumaduan[flag1]);
                cnt=0;
            }
        }
        if (0==value2)
        {
            usleep(400000);
            if (0==value2)
            {
                flag2++;
                printf("flag2 : %d \n", flag2);
#ifdef HSV7011 //ip = 1-24
                if ((2 == flag1) && (flag2 > 4)) //num > 24
                {
                    
                    flag2 = 0;
                }
                if ((0 == flag1) && (flag2 > 9)) //01 -09
                {
                    flag2 = 1;
                }
                if (flag2 > 9)
                {
                    flag2 = 0;
                }
#endif
#ifdef HSV7012 //ip = 1-128
                if ((0 == flag1) && (flag2 > 9)) //1 - 9
                {
                    flag2 = 1;
                }
                if ((12 == flag1) && (flag2 > 8)) //num > 128
                {
                    flag2 = 0;
                }
                if (flag2 > 9)
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
        usleep(100000);  
        cnt++;
        if(cnt>50)    //5s
        {
            tmp_add = (flag1*10)+flag2;
            //printf("tmp_add : %d \n", tmp_add);
            //printf("mul_add : %d \n", mul_add);
            if(mul_add!=tmp_add)
            {
                //printf("old_ip != new_ip;  fa song new_ip ! \n");
                //printf("ip_address:%d , tmp:%d\n",ip_address,tmp);
                mul_add = tmp_add;

                sprintf(str, "239.255.42.%d", tmp_add); //multicast address
                strcpy(share_mem->sm_eth_setting.strEthMulticast, str);
                printf("str: %s", str);
#ifdef HSV7011
                sprintf(str, "192.168.1.%d", tmp_add+200); //201 - 224
                strcpy(share_mem->sm_eth_setting.strEthIp, str);
                printf("str :%s\n",str);
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
                share_mem->ucUpdateFlag = 1;
                sleep(1);
                while (share_mem->ucUpdateFlag)
                    sleep(1);
                reboot1();
                //printf("newip write is ok!\n");
            }
        }
    }// while

#endif
}
#if 0
void *app_tx_light_ctl_main(void)
{
    unsigned int i=0;
	//int i;
  	
  	//SL_U32 sum;
	SL_U32 Value;
	
#if 0
	//LED
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
#endif
#if 0
	//HDMI
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
	
    g_LightMode[0].uLightNo = 4;
    g_LightMode[1].uLightNo = 5;
    g_LightMode[2].uLightNo = 6;
    g_LightMode[3].uLightNo = 7;
    
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
#if 1 	/********************* for wang 20171024 ******************/ 
		int flag=0;
	    int time=0;
	    char ret;
	    
	    
	    if(0 == get_key_value())
	    {
			Sleep(5);
			
			if(0 == get_key_value())
			{
				flag=1;
	           
	            do{
					Value = get_key_value();		  
					time = time+1;
					Sleep(1);
					
					/**for wang */
					if(time > 1500)
					{
						flag = 3;
						
						AppInitCfgInfoDefault();
						printf("cfg init ok \n");
						AppWriteCfgInfotoFile();
						printf("------------reboot---------------------------------");
						Sleep(10);
						reboot1();
					}
		        }while (Value == 0);
				/*
		        if(time > 1500)
		        {
					flag = 3;
		        }
		        */
                for (i=1;i<500;i++)
		        {
            	    Sleep(1);		   
			        Value = get_key_value();
		            if(Value ==0)
			        {
				        Sleep(10);
	      			    Value = get_key_value();
	      			    AppInitCfgInfoDefault();
						printf("cfg init ok \n");
						AppWriteCfgInfotoFile();
	     			    if(Value == 0)
				        {
				            flag=2;
			            }	
		            }
		         }
		         printf("FALG is %d\n", flag);	
	        }
	    }
      
        switch(flag)
        {
			case 1:  //  Change wifi mode
	        {	
				#if 0
				printf("wifi mode change \n");
				ret = Wifi_mode_change();
		        if (ret)
					printf("wifi mode change success \n");
				else
					printf("wifi mode change error \n");
		        #endif
		        break;
	        }
			case 3:  //reboot
			{
				reboot1();
				break;
			}
			default:
				break;
		}
		Sleep(100);
#endif /*************************************************************/

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

void HDMI_light_off(void)
{
#if 0
    SetLightMode(HDMI_LED, LED_OFF, 0);
    //GPIO_setDir(HDMI_LED, GPIO_INPUT);
#else
    GPIO_export(LED_HDMI);
    GPIO_setDir(LED_HDMI, GPIO_INPUT); //output
    GPIO_setValue(LED_HDMI, GPIO_HIG_STA); //high
#endif
}

void HDMI_light_on(void)
{
#if 0
    SetLightMode(HDMI_LED, LED_ON, 0);
    //GPIO_setDir(HDMI_LED, GPIO_OUTPUT);
#else
    GPIO_export(LED_HDMI);	   			
    GPIO_setDir(LED_HDMI, GPIO_OUTPUT); //output
    GPIO_setValue(LED_HDMI, GPIO_LOW_STA); //
#endif
}

#if 1
void GPIO_init(void)
{
    //system running state led
    GPIO_export(LED_SYS_STA);	   			
    GPIO_setDir(LED_SYS_STA, GPIO_OUTPUT); //output
    GPIO_setValue(LED_SYS_STA, GPIO_LOW_STA); //

    //SWITCH 
    GPIO_openFd(MULTICAST_SWITCH_1);
    GPIO_export(MULTICAST_SWITCH_1);	   			
    GPIO_setDir(MULTICAST_SWITCH_1, GPIO_INPUT);

    GPIO_openFd(MULTICAST_SWITCH_2);
    GPIO_export(MULTICAST_SWITCH_2);	   			
    GPIO_setDir(MULTICAST_SWITCH_2, GPIO_INPUT);

    GPIO_openFd(MULTICAST_SWITCH_3);
    GPIO_export(MULTICAST_SWITCH_3);	   			
    GPIO_setDir(MULTICAST_SWITCH_3, GPIO_INPUT);

    GPIO_openFd(MULTICAST_SWITCH_4);
    GPIO_export(MULTICAST_SWITCH_4);	   			
    GPIO_setDir(MULTICAST_SWITCH_4, GPIO_INPUT);

    GPIO_openFd(MULTICAST_SWITCH_5);
    GPIO_export(MULTICAST_SWITCH_5);	   			
    GPIO_setDir(MULTICAST_SWITCH_5, GPIO_INPUT);
/*
    GPIO_openFd(MULTICAST_SWITCH_6);
    GPIO_export(MULTICAST_SWITCH_6);	   			
    GPIO_setDir(MULTICAST_SWITCH_6, GPIO_INPUT);

    GPIO_openFd(MULTICAST_SWITCH_7);
    GPIO_export(MULTICAST_SWITCH_7);	   			
    GPIO_setDir(MULTICAST_SWITCH_7, GPIO_INPUT);
*/
}
#endif

SL_U32 get_key_value(void)
{
    SL_U32 value;
	GPIO_getValue(KEY_IO, &value);
    return value;
}

#if 1
void Init_Multicast_and_IP(void)
{
	SL_U32 value = 0, tmp1, tmp2, i = 10;
	char str[50] = {0};
	
	printf("-----------IP switch-----------\n");
	sleep(2);
	//while(1)
	{
	tmp1 = 0x00;
	
	GPIO_getValue(MULTICAST_SWITCH_1, &value); //1
	tmp1 |= value; //0x01
	//printf("value: %d \n", value);
	tmp1 = tmp1 << 1; //0x02
	GPIO_getValue(MULTICAST_SWITCH_2, &value); //1
	tmp1 = tmp1 | value; //0x03
	//printf("value: %d \n", value);
	tmp1 = tmp1 << 1;
	GPIO_getValue(MULTICAST_SWITCH_3, &value);
	tmp1 |= value;
	tmp1 = tmp1 << 1;
	GPIO_getValue(MULTICAST_SWITCH_4, &value);
	tmp1 |= value;
	tmp1 = tmp1 << 1;
	GPIO_getValue(MULTICAST_SWITCH_5, &value);
	tmp1 |= value;
	
	#if 0
	tmp1 = tmp1 << 1;
	GPIO_getValue(MULTICAST_SWITCH_6, &value);
	tmp1 |= value;
	tmp1 = tmp1 << 1;
	GPIO_getValue(MULTICAST_SWITCH_7, &value);
	tmp1 |= value;
	#endif
	
	printf("tmp1 = 0x%x \n", tmp1);
	//tmp1+=1;
	sprintf(str, "239.255.42.%d", tmp1); //multicast address
	strcpy(share_mem->sm_eth_setting.strEthMulticast, str);
	printf(str);
	sprintf(str, "192.168.1.%d", tmp1+200); //ip address 
	strcpy(share_mem->sm_eth_setting.strEthIp, str);
	printf(str);
	share_mem->ucUpdateFlag = 1;
	
	usleep(10000);
	}
}
#endif


void device_reset(void)
{
    printf("device reset \n\n");
    GPIO_openFd(DEVICE_RESET);
    GPIO_export(DEVICE_RESET);
    GPIO_setDir(DEVICE_RESET, GPIO_OUTPUT);
    GPIO_setValue(DEVICE_RESET, GPIO_HIG_STA);
    usleep(10000);
    GPIO_setValue(DEVICE_RESET, GPIO_LOW_STA);
    usleep(100000);
    GPIO_setValue(DEVICE_RESET, GPIO_HIG_STA);
}




#if 0
void Multicast_switch(void)
{
    SL_U32 value = 0, tmp1, tmp2, i = 10;
    char str[50] = {0};

    printf("-----------IP switch-----------\n");
    while (1)
    {
        tmp1 = 0x00;

        GPIO_getValue(MULTICAST_SWITCH_1, &value); //1
        tmp1 |= value; //0x01
        printf("value: %d \n", value);
        tmp1 = tmp1 << 1; //0x02
        GPIO_getValue(MULTICAST_SWITCH_2, &value); //1
        tmp1 = tmp1 | value; //0x03
        printf("value: %d \n", value);
        tmp1 = tmp1 << 1;
        GPIO_getValue(MULTICAST_SWITCH_3, &value);
        tmp1 |= value;
        tmp1 = tmp1 << 1;
        GPIO_getValue(MULTICAST_SWITCH_4, &value);
        tmp1 |= value;
        tmp1 = tmp1 << 1;
        GPIO_getValue(MULTICAST_SWITCH_5, &value);
        tmp1 |= value;

#if 0
        tmp1 = tmp1 << 1;
        GPIO_getValue(MULTICAST_SWITCH_6, &value);
        tmp1 |= value;
        tmp1 = tmp1 << 1;
        GPIO_getValue(MULTICAST_SWITCH_7, &value);
        tmp1 |= value;
#endif

        printf("tmp1 = 0x%x \n", tmp1);


#if 1
        if (tmp2 != tmp1)
        {
            tmp2 = tmp1;
            sprintf(str, "239.255.42.%d", tmp1+200); //multicast address
            strcpy(share_mem->sm_eth_setting.strEthMulticast, str);
            printf(str);
            sprintf(str, "192.168.1.%d", tmp1+200); //ip address 
            strcpy(share_mem->sm_eth_setting.strEthIp, str);
            printf(str);
            AppWriteCfgInfotoFile();
            init_eth();
        }
#endif
        usleep(10000);
    }

}
#endif

