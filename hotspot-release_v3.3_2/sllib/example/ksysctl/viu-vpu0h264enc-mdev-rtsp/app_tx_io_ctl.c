
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <sl_gpio.h>

#include <sl_types.h>

#include "app_tx_io_ctl.h"

#include "wifi_ap.h"
#include "cfginfo.h" 
#include "sharemem.h"

#define SYSFS_GPIO_DIR  "/sys/class/gpio"

extern SL_BOOL gbTestMode;


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

void KVM_REST(void)
{
	GPIO_export(KVMRST_IO);
	GPIO_setDir(KVMRST_IO, GPIO_OUTPUT);
	GPIO_setValue(KVMRST_IO, GPIO_HIG_STA);
	usleep(100);
	GPIO_setValue(KVMRST_IO, GPIO_LOW_STA);
}

SL_U32 get_key_value(void)
{
	SL_U32 value;
	
	GPIO_getValue(KEY_IO, &value);
	
	return value;
}

