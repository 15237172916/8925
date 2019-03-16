#ifndef _APP_TX_IO_H
#define _APP_TX_IO_H


#include <sl_types.h>

void  *app_tx_io_ctl_main(void);

typedef struct
{
	SL_BOOL bShowFlag; //SL_FALSE not show, SL_TRUE show
	unsigned char uShowTime;
	char *cText;
}SHOWDATA;

SHOWDATA g_ShowData;


SL_BOOL gbTestMode;

SL_S32 GPIO_setValue(SL_U32 Gpio, SL_U32 Value);

void *app_tx_light_ctl_main(void);

int reboot1(void);

typedef struct
{
    unsigned char uLightNo;
    unsigned char uMode;//0:on 1:off 2:flash
    unsigned char uInterval;//100ms unit
    unsigned char uCnt;    //current count
    unsigned char uCStatus;//current status 0:on 1:off
}LIGHTMODE;

LIGHTMODE g_LightMode[4]; //0:D32 1:D33 2:D34 3:D35

void SetLightMode(unsigned char uLightNo, unsigned char uMode, unsigned char uCnt);

#define HDMI_LED 3
#define HIGH_LED 1
#define MID_LED 0
#define LOW_LED 2


#define LED_ON 0
#define LED_OFF 1

#define KEY_IO		8 		//GPIO1_8

#define LED_SIG_LOW 	6 	//GPIO1_6
#define LED_SIG_MOD		4	//GPIO1_4
#define LED_SIG_HIG		5	//GPIO1_5
#define LED_HDMI		7	//GPIO1_7
#define LED_SYS_STA     30  //GPIO1_30

#define GPIO_OUTPUT		1	//gpio mode is output
#define GPIO_INPUT		0	//gpio mode is input

#define GPIO_HIG_STA	1	//gpio output mode is high 
#define GPIO_LOW_STA	0	//gpio output mode is low


#if 1 //
//Multicast address switch io port
#define MULTICAST_SWITCH_1		34 	//GPIO02_02
#define MULTICAST_SWITCH_2		36	//GPIO02_04

#define MULTICAST_SWITCH_3		4	//GPIO01_04
#define MULTICAST_SWITCH_4		7	//GPIO01_07
#define MULTICAST_SWITCH_5		6	//GPIO01_06
#define MULTICAST_SWITCH_6		5	//GPIO01_05
#define MULTICAST_SWITCH_7		30	//GPIO01_30
#endif




void HDMI_light_off(void);
void HDMI_light_on(void);

SL_U32 get_key_value(void);
void Init_Multicast_and_IP(void);

#endif
