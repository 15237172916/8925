#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sl_param_vcodec.h>
#include <sl_types.h>
#include <sl_rtsp.h>
#include <linux/soundcard.h>
#include <sys/mman.h>

#include "drv_sii9293_register.h"
#include "drv_sii9293.h"
#include "stdio.h"
#include "audio_ioctl.h"

//#define APP_CODE

#ifdef APP_CODE
#include "app_tx_io_ctl.h"
#include "app_tx_data_ch.h"
#endif

unsigned char HDMI_lost;

audio_dma_info_s audio_dma_in;
audio_dma_info_s audio_dma_out;

int audioIn_trigger = 0;
int audioOut_trigger = 0;

	
int audio_configed = 0;

extern H264_APPEND_INFO_s h264_append_info;

#define I2C_RETRIES 0x0701
#define I2C_TIMEOUT 0x0702
#define I2C_RDWR 0X0707

#define	I2C_M_WR 0
struct i2c_msg {
	unsigned short addr;	
	unsigned short flags;
#define I2C_M_TEN		0x0010	
#define I2C_M_RD		0x0001	
#define I2C_M_NOSTART		0x4000	
#define I2C_M_REV_DIR_ADDR	0x2000	
#define I2C_M_IGNORE_NAK	0x1000	
#define I2C_M_NO_RD_ACK		0x0800	
#define I2C_M_RECV_LEN		0x0400	
	unsigned short len;		
	unsigned char *buf;		
};

struct i2c_rdwr_ioctl_data {
	struct i2c_msg *msgs;	
	int nmsgs;			
};


int rdfd_dsp = -1;
int fd_w = -1;
int audio_fd = -1;

static int fd; //i2c 

struct i2c_rdwr_ioctl_data e2prom_data;

#define Fs_44p1_kHz 	0
#define Fs_48_kHz 	 2
#define Fs_32_kHz 		3
#define Fs_22p05_kHz 4
#define Fs_24_kHz 		6
#define Fs_88p2_kHz 	8
#define Fs_768_kHz 	9
#define Fs_96_kHz 		10
#define Fs_176p4_kHz 12
#define Fs_192_kHz 	14

static void sii9293_HAL_SetChipAddr(UINT8 u8_address);
static void sii9293_HAL_WriteByte(UINT16 u16_index, UINT8 u8_value);
static void sii9293_HAL_ModBits(UINT16 u16_index, UINT8 u8_mask, UINT8 u8_value);
static void sii9293_HAL_SetBits(UINT16 u16_index, UINT8 u8_mask);
static void sii9293_HAL_ClrBits(UINT16 u16_index, UINT8 u8_mask);
static UINT8 I2C_ReadByte(unsigned int addr, unsigned char regOffset);

UINT8 sii9293_HAL_ReadByte(UINT16 u16_index);

//Device Addr Page
static int Page[] = {
	0x64, 0x68, 0x74, 0x80, 0xC0, 0xD0, 0xE0
};

static UINT8 sii9293_u8chipAddr = SYSTEM;

AUDIO_CAPS AudioCaps;

video_info_s video_info_t;

sii9293_chip_s *chip;


/*
	*****************************************************************************
*/

#if 0
void sii9293_init_register(void)
{
	int ID_Hbyte, ID_Lbyte;
INIT:
	/*
		read device only ID
	*/
	printf("9293 init start \n");
	
	sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection

#if 1
	ID_Hbyte = sii9293_HAL_ReadByte(DEV_IDH);
	ID_Lbyte = sii9293_HAL_ReadByte(DEV_IDL);
	printf("\n\n  ******ID_Hbyte:%x\n",ID_Hbyte);
	printf("ID_Lbyte:%x\n",ID_Lbyte);
#endif
#if 0
	while(1)
	{
		//ID_Hbyte = I2C_ReadByte(0x64, DEV_IDH);
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		ID_Hbyte = sii9293_HAL_ReadByte(DEV_IDH);
		ID_Lbyte = sii9293_HAL_ReadByte(DEV_IDL);
		printf("\n\n  ******ID_Hbyte:%x\n",ID_Hbyte);
		printf("ID_Lbyte:%x\n",ID_Lbyte);
		sleep(1);
	}
#endif
	if (ID_Hbyte == 0x92 && ID_Lbyte == 0x93)
	{
		/*
			System config 
		*/
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		sii9293_HAL_WriteByte(SRST, 0x17);	//Software Reset Register
		
		sii9293_HAL_SetChipAddr(Page[5]);	//Device Selection
		sii9293_HAL_WriteByte(TMDS_TERMCTRL, 0xC0);	//enable rx core , 
		
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		sii9293_HAL_WriteByte(HOT_PLUG_CTRL, 0x00); //low
		
		/*
			AVI input video and audio config 
		*/
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		
		//sii9293_HAL_WriteByte(AEC0_CTRL, 0x21);
		//sii9293_HAL_WriteByte(AEC0_CTRL, 0x20);	//Output color space is YCbCr
		sii9293_HAL_WriteByte(INTR3_MASK, 0x05);	//Enable audio changed and AVI changed
		sii9293_HAL_WriteByte(IP_CTRL, 0x01);	//Set interrupt on any received AVI InfoFrame
		//sii9293_HAL_WriteByte(VID_CTRL, 0x01);	//BT.709
		sii9293_HAL_WriteByte(VID_MODE2, 0x02);	//hotspot Output color space is YCbCr
		//sii9293_HAL_WriteByte(VID_MODE2, 0x22);	//Output color space is YCbCr
		sii9293_HAL_WriteByte(VID_MODE, 0x0A); //hotspot	Convert RGB to YUV, 444 to 422
		//sii9293_HAL_WriteByte(VID_MODE, 0x4a);	//RGB
		//sii9293_HAL_WriteByte(VID_MODE, 0x20);	//YUV444
		//sii9293_HAL_WriteByte(VID_MODE, 0x00);	//YUV422
		//sii9293_HAL_WriteByte(VID_AOF, 0xC0);	//Digital 8-bit 4:2:2 YCbCr output
		
		printf("\n\n***VID_MODE:%x\n",sii9293_HAL_ReadByte(VID_MODE));
		printf("***AEC0_CTRL:%x\n",sii9293_HAL_ReadByte(AEC0_CTRL));
		printf("***VID_CTRL:%x\n",sii9293_HAL_ReadByte(VID_CTRL));
		printf("***VID_MODE2:%x\n",sii9293_HAL_ReadByte(VID_MODE2));
		printf("***VID_AOF:%x\n",sii9293_HAL_ReadByte(VID_AOF));

/*
		sii9293_HAL_WriteByte(AEC0_CTRL, 0x21);
		sii9293_HAL_WriteByte(VID_MODE, 0x5A);	//Convert RGB to YUV, 444 to 422
		sii9293_HAL_WriteByte(VID_CTRL, 0x01);	//BT.709
		sii9293_HAL_WriteByte(VID_MODE2, 0x02);	//Output color space is YCbCr
		//ret = sii9293_HAL_ReadByte(VID_MODE);
		printf("\n ***VID_MODE: %x ****\n", sii9293_HAL_ReadByte(VID_MODE));
		printf("\n ***VID_MODE2: %x ****\n", sii9293_HAL_ReadByte(VID_MODE2));
		printf("\n ***VID_CTRL: %x **** \n", sii9293_HAL_ReadByte(VID_CTRL));
*/	


		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(AVI_TYPE, 0x82);	//AVI InfoFrame Packet Type Code.(Expected)
		sii9293_HAL_WriteByte(AVI_VERS, 0x02);	//AVI InfoFrame Packet Version Code. (Expected)
		sii9293_HAL_WriteByte(AVI_LEN, 0x0D);	//AVI InfoFrame Packet Length.(Expected)
		
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(INTR5_MASK, 0x40);	//Polarity change on Vsync or Hsync
		sii9293_HAL_WriteByte(INTR6_MASK, 0x04);	//New or changed ACP packet detected
		sii9293_HAL_WriteByte(RX_INTR8_MASK, 0x20);	//Indicating the HDMI cable is plugged in (PWR5V is HIGH)
		sii9293_HAL_WriteByte(AEC0_CTRL, 0x00);
		sii9293_HAL_WriteByte(AEC0_CTRL, 0x24);//0x24	//Enable AAC to turn off SD0, SD1, SD2, SD3, SCK, and SPDIF. Enable AVC
		sii9293_HAL_WriteByte(SRST, 0x20);	//Reset (flush) FIFO
		sii9293_HAL_WriteByte(SRST, 0x00);
		sii9293_HAL_WriteByte(AEC0_CTRL, 0x25);	//Enable AAC to turn off SD0, SD1, SD2, SD3, SCK, and SPDIF
																						//Enable AVC
																						//Enable AAC
		//Audio Clock Recovery config
		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(ACR_CTRL1, 0x01);	//Set this bit to 1 to generate the strobe.
		
		//Receiver Control config
		sii9293_HAL_SetChipAddr(Page[5]);
		sii9293_HAL_WriteByte(RX_CTRL1, 0x3F);	//All ON
		
		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(ACR_CTRL3, 0x54);	//
		
		//sii9293_HAL_SetChipAddr(Page[0]);
		//sii9293_HAL_WriteByte(RX_SLAVE_ADDR5, 0x54);
		
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(BCH_THRESH, 0x02);	//BCH Threshold
		sii9293_HAL_WriteByte(ECC_CTRL, 0x01);	// Clear error counters used within the AEC logic to flag audio packetre ception errors before they overflow
		sii9293_HAL_WriteByte(SYS_SWTCHC, 0x91);	//Enable the DDC delay. Enable DDC 0 (valid only if DDC 1 is disabled).Enable Core 0 (valid only if Core 1 is disabled)
		//sii9293_HAL_WriteByte();
		/*
			Audio config 
		*/
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(ACR_CFG1, 0x88);	//ACR PLL Configuration Byte 1
		sii9293_HAL_WriteByte(ACR_CFG2, 0x16);	//ACR PLL Configuration Byte 2
		
		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(AUD_CTRL, 0x14); //for wang 20171222
		//sii9293_HAL_WriteByte(AUD_CTRL, 0x3B);
		//sii9293_HAL_WriteByte(I2S_CTRL2, 0x1C);	//Channel enabled .The 9293A uses only SD0.  Enable MCLKOUT
		
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(RX_SRST2, 0x40);	//Auto audio FIFO reset whenever audio FIFO overflows or under-runs
		sii9293_HAL_WriteByte(HOT_PLUG_CTRL, 0x01);	// hight
		
		//sii9293_HAL_WriteByte(SYS_CTRL1, 0x05);	//24/30/36-bit mode ,  Normal operation
		
		printf("9293 init success \n");
	}
	else 
	{
		printf("9293 device error \n");
		goto INIT;
	}
}

#endif

#if 1
void sii9293_init_register(void)
{
	int ID_Hbyte, ID_Lbyte;
INIT:
	/*
		read device only ID
	*/
	printf("9293 init start \n");
	
	sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection

#if 1
	ID_Hbyte = sii9293_HAL_ReadByte(DEV_IDH);
	ID_Lbyte = sii9293_HAL_ReadByte(DEV_IDL);
	printf("\n\n  ******ID_Hbyte:%x\n",ID_Hbyte);
	printf("ID_Lbyte:%x\n",ID_Lbyte);
#endif
#if 0
	while(1)
	{
		//ID_Hbyte = I2C_ReadByte(0x64, DEV_IDH);
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		ID_Hbyte = sii9293_HAL_ReadByte(DEV_IDH);
		ID_Lbyte = sii9293_HAL_ReadByte(DEV_IDL);
		printf("\n\n  ******ID_Hbyte:%x\n",ID_Hbyte);
		printf("ID_Lbyte:%x\n",ID_Lbyte);
		sleep(1);
	}
#endif
	if (ID_Hbyte == 0x92 && ID_Lbyte == 0x93)
	{
		/*
			System config 
		*/
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		sii9293_HAL_WriteByte(SRST, 0x17);	//Software Reset Register
		
		sii9293_HAL_SetChipAddr(Page[5]);	//Device Selection
		sii9293_HAL_WriteByte(TMDS_TERMCTRL, 0xC0);	//enable rx core , 
		
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		sii9293_HAL_WriteByte(HOT_PLUG_CTRL, 0x00); //low
		
		/*
			AVI input video and audio config 
		*/
		sii9293_HAL_SetChipAddr(Page[0]);	//Device Selection
		sii9293_HAL_WriteByte(INTR3_MASK, 0x05);	//Enable audio changed and AVI changed
		sii9293_HAL_WriteByte(IP_CTRL, 0x01);	//Set interrupt on any received AVI InfoFrame
		sii9293_HAL_WriteByte(VID_CTRL, 0x01);	//BT.709
		sii9293_HAL_WriteByte(VID_MODE2, 0x02);	//Output color space is YCbCr
		sii9293_HAL_WriteByte(VID_MODE, 0x0A);	//Convert RGB to YUV, 444 to 422
		sii9293_HAL_WriteByte(VID_AOF, 0xC0);	//Digital 8-bit 4:2:2 YCbCr output
		
		
		/*
		sii9293_HAL_WriteByte(AEC0_CTRL, 0x21);
		sii9293_HAL_WriteByte(VID_MODE, 0x5A);	//Convert RGB to YUV, 444 to 422
		sii9293_HAL_WriteByte(VID_CTRL, 0x01);	//BT.709
		sii9293_HAL_WriteByte(VID_MODE2, 0x02);	//Output color space is YCbCr
		//ret = sii9293_HAL_ReadByte(VID_MODE);
		printf("/n ***VID_MODE: %x ****\n", sii9293_HAL_ReadByte(VID_MODE));
		printf("/n ***VID_MODE2: %x ****\n", sii9293_HAL_ReadByte(VID_MODE2));
		printf("/n ***VID_CTRL: %x **** \n", sii9293_HAL_ReadByte(VID_CTRL));
		*/
		
		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(AVI_TYPE, 0x82);	//AVI InfoFrame Packet Type Code.(Expected)
		sii9293_HAL_WriteByte(AVI_VERS, 0x02);	//AVI InfoFrame Packet Version Code. (Expected)
		sii9293_HAL_WriteByte(AVI_LEN, 0x0D);	//AVI InfoFrame Packet Length.(Expected)
		
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(INTR5_MASK, 0x40);	//Polarity change on Vsync or Hsync
		sii9293_HAL_WriteByte(INTR6_MASK, 0x04);	//New or changed ACP packet detected
		sii9293_HAL_WriteByte(RX_INTR8_MASK, 0x20);	//Indicating the HDMI cable is plugged in (PWR5V is HIGH)
		//sii9293_HAL_WriteByte(AEC0_CTRL, 0x24);//0x24	//Enable AAC to turn off SD0, SD1, SD2, SD3, SCK, and SPDIF. Enable AVC
		sii9293_HAL_WriteByte(AEC0_CTRL, 0xC5); //auto config audio and video
		sii9293_HAL_WriteByte(SRST, 0x20);	//Reset (flush) FIFO
		sii9293_HAL_WriteByte(SRST, 0x00);
		//sii9293_HAL_WriteByte(AEC0_CTRL, 0x25);	//Enable AAC to turn off SD0, SD1, SD2, SD3, SCK, and SPDIF
																						//Enable AVC
																						//Enable AAC
		//Audio Clock Recovery config
		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(ACR_CTRL1, 0x01);	//Set this bit to 1 to generate the strobe.
		
		//Receiver Control config
		sii9293_HAL_SetChipAddr(Page[5]);
		sii9293_HAL_WriteByte(RX_CTRL1, 0x3F);	//All ON
		
		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(ACR_CTRL3, 0x54);	//
		
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(RX_SLAVE_ADDR5, 0x54);
		
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(BCH_THRESH, 0x02);	//BCH Threshold
		sii9293_HAL_WriteByte(ECC_CTRL, 0x01);	// Clear error counters used within the AEC logic to flag audio packetre ception errors before they overflow
		sii9293_HAL_WriteByte(SYS_SWTCHC, 0x91);	//Enable the DDC delay. Enable DDC 0 (valid only if DDC 1 is disabled).Enable Core 0 (valid only if Core 1 is disabled)
		//sii9293_HAL_WriteByte();
		/*
			Audio config 
		*/
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(ACR_CFG1, 0x88);	//ACR PLL Configuration Byte 1
		sii9293_HAL_WriteByte(ACR_CFG2, 0x16);	//ACR PLL Configuration Byte 2
		
		sii9293_HAL_SetChipAddr(Page[1]);
		sii9293_HAL_WriteByte(AUD_CTRL, 0x14); //for wang 20171222
		//sii9293_HAL_WriteByte(AUD_CTRL, 0x3B);
		sii9293_HAL_WriteByte(I2S_CTRL2, 0xFC);	//SD0 Channel enabled .The 9293A uses only SD0.  Enable MCLKOUT
		
		sii9293_HAL_SetChipAddr(Page[0]);
		sii9293_HAL_WriteByte(RX_SRST2, 0x40);	//Auto audio FIFO reset whenever audio FIFO overflows or under-runs
		sii9293_HAL_WriteByte(HOT_PLUG_CTRL, 0x01);	// hight
		
		sii9293_HAL_WriteByte(SYS_CTRL1, 0x05);	//24/30/36-bit mode ,  Normal operation
		
		printf("9293 init success \n");
	}
	else 
	{
		printf("9293 device error \n");
		goto INIT;
	}
}

 #endif

/*
	HDMI/MHL cable-in/cable-out
*/
void sii9293_HDMI_CableStatus(UINT8 *hdmi_out, UINT8 *hdmi_in)
{
	sii9293_HAL_SetChipAddr(Page[0]);		//page select
	
	// Check HDMI cable in/out
	*hdmi_out = sii9293_HAL_ReadByte(INTR6) & BIT1;
	*hdmi_in = sii9293_HAL_ReadByte(RX_INTR8) & BIT2;
	
	// Clear 5v plug-in / plug-out interrupts if any
	sii9293_HAL_ModBits(INTR6, BIT1, *hdmi_out);
	sii9293_HAL_ModBits(RX_INTR8, BIT2, *hdmi_in);
	
	// Per mask cannot mask out the two interrupts
	*hdmi_out &= sii9293_HAL_ReadByte(INTR6_MASK) & BIT2;
	*hdmi_in &= sii9293_HAL_ReadByte(RX_INTR8_MASK) & BIT1;
}

/*
		***State of the HDMI port 5 V power detect pin
*/
int Is_HDMI_5V_detected(void)
{
	unsigned char value = 0;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(STATE) & BIT3;	
	//printf("HDMI_5V : %x \n", value);
	if (value)
		return 1;
	else
	{
		printf("HDMI_5V : %x \n", value);
		return 0;
	}
}

/*
	**State of the SYNC Detect.
*/
int Is_Sync_stable(void)
{
	unsigned char value = 0;
	unsigned char mask = 0;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(STATE);
	
	mask = BIT0 | BIT1;
	if (value & mask)
		return 1;
	else      
		return 0;
}

int Is_eccErr_occur(void)
{
	unsigned char value;
	int cnt = 0;
	
try:
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(INTR1_MASK);
	value |= BIT2;
	sii9293_HAL_WriteByte(INTR1_MASK, value);
	value = sii9293_HAL_ReadByte(INTR1);
	//printf("value : %x\n", value);
	if (value & BIT2)
	{
		printf("ecc error\n");
		printf("value & BIT2 :%d \n", value & BIT2);
		value = sii9293_HAL_ReadByte(INTR1);
		sii9293_HAL_WriteByte(INTR1, value | BIT2);
//		value = sii9293_HAL_ReadByte(INTR1);
//		printf("INTR1 : ", value);
		//mculib_delay_ms(100);
		usleep(100000);
		value = sii9293_HAL_ReadByte(INTR1);
		printf("INTR1 : %x\n", value);
		cnt ++;
		if (cnt >= 10)
			return 1;
		goto try;
	}
	
	return 0;
}


/*
	**
*/
void sii9293_HDCP_init(void)
{
	sii9293_HAL_SetChipAddr(Page[0]);
	sii9293_HAL_ReadByte(SRST);

	sii9293_HAL_WriteByte(SRST, 0x08);	//HDCP reset
	sii9293_HAL_WriteByte(SRST, 0x00);
	
	sii9293_HAL_WriteByte(INTR1, 0x01);
}
/*
	***audio 
*/

static void Audio_init(void)
{
	
#if 1 //audio config
	
	sii9293_HAL_SetChipAddr(Page[1]);
	
	sii9293_HAL_WriteByte(ACR_CTRL1, 0x01);
	sii9293_HAL_WriteByte(ACR_CTRL3, 0x54);
	//sii9293_HAL_WriteByte(I2S_CTRL1, 0x40);
	//sii9293_HAL_WriteByte(I2S_CTRL2, 0xf4); //
	//sii9293_HAL_WriteByte(I2S_MAP, );
	//sii9293_HAL_WriteByte(AUD_CTRL, );

	//sii9293_HAL_WriteByte(AEC0_CTRL, 0x00); // disable any Auto Config

	// Page 2 - System Control and Status
	sii9293_HAL_SetChipAddr(Page[1]);
	//audio IIS 
	sii9293_HAL_WriteByte(I2S_CTRL1, 0x60); //16bit
	//sii9293_HAL_WriteByte(I2S_CTRL1, 0x40);	//32bit
    sii9293_HAL_WriteByte(I2S_CTRL2, 0x1c);	//The SiI9293A uses only SD0
    sii9293_HAL_WriteByte(I2S_MAP, 0xe4);
    
    //audio clock
    sii9293_HAL_WriteByte(TCLK_FS, 0x12);
	//sii9293_HAL_WriteByte(ACR_CTRL, 0x02);
	sii9293_HAL_WriteByte(AUD_CTRL, 0x14);
	sii9293_HAL_WriteByte(ACR_CTRL1, 0x02);
	sii9293_HAL_WriteByte(ACR_CTRL3, 0x54);
	sii9293_HAL_WriteByte(TDM_CTRL1, 0x0e);
	sii9293_HAL_WriteByte(TDM_CTRL2, 0x01);
	sii9293_HAL_WriteByte(AUDP_MUTE, 0x00);
	//sii9293_HAL_WriteByte(AUDP_CRIT2, 0x0c);
	sii9293_HAL_WriteByte(AUDP_FIFO, 0x0c);
	
#endif

#if 0

	#define RX_A__DSD_MUTE_SEQ		0x25 // DSD mute sequence
	//sii9293_HAL_WriteByte(RX_A__DSD_MUTE_SEQ, );
	printf("###################################################################\n");
	printf("RX_A__I2S_CTRL1:0x%x\n",sii9293_HAL_ReadByte(I2S_CTRL1));
	printf("RX_A__I2S_CTRL2:0x%x\n",sii9293_HAL_ReadByte(I2S_CTRL2));
	printf("RX_A__I2S_MAP:0x%x\n",sii9293_HAL_ReadByte(I2S_MAP));
	printf("RX_A__PCLK_FS:0x%x\n",sii9293_HAL_ReadByte(TCLK_FS));
	
	printf("RX_A__PD_SYS3:0x%x\n",sii9293_HAL_ReadByte(PD_SYS3));
	printf("RX_A__AUDRX_CTRL:0x%x\n",sii9293_HAL_ReadByte(AUD_CTRL));
	
	printf("RX_A__TDM_CTRL1:0x%x\n",sii9293_HAL_ReadByte(TDM_CTRL1));
	printf("RX_A__TDM_CTRL2:0x%x\n",sii9293_HAL_ReadByte(TDM_CTRL2));

	printf("RX_A__ACR_CTRL1:0x%x\n",sii9293_HAL_ReadByte(ACR_CTRL1));
	printf("RX_A__ACR_CTRL3:0x%x\n",sii9293_HAL_ReadByte(ACR_CTRL3));
	printf("RX_A__FREQ_SVAL:0x%x\n",sii9293_HAL_ReadByte(FREQ_SVAL));

	printf("RX_A__DSD_MUTE_SEQ:0x%x\n",sii9293_HAL_ReadByte(RX_A__DSD_MUTE_SEQ));
	printf("RX_A__AUDP_MUTE:0x%x\n",sii9293_HAL_ReadByte(AUDP_MUTE));
	printf("RX_A__AUDP_FIFO:0x%x\n",sii9293_HAL_ReadByte(AUDP_FIFO));
	
#if 0
	printf(":0x%x\n",SiiRegRead());
	printf(":0x%x\n",SiiRegRead());
	printf(":0x%x\n",SiiRegRead());
	printf(":0x%x\n",SiiRegRead());
#endif
	
	printf("###################################################################\n");
#endif
	
}


#define AUD_FS 0x0f

void getHDMIRXInputAudio(AUDIO_CAPS *pAudioCaps)
{
    UINT8 uc = 0;
	UINT8 frequency = 0;
	
    if(!pAudioCaps)
    {
        return ;
    }
    sii9293_HAL_SetChipAddr(Page[1]);
	uc = sii9293_HAL_ReadByte(ACR_CTRL1);
	uc = (~uc) | BIT1;
	sii9293_HAL_WriteByte(ACR_CTRL1, ~uc); //Audio sample frequency select  hardware-determined fs value

	frequency = sii9293_HAL_ReadByte(CHST4);
	pAudioCaps->SampleFreq = frequency & AUD_FS;
		
		
		
}
#define Fs_44p1_kHz 0
#define Fs_48_kHz 	 2
#define Fs_32_kHz 		3
#define Fs_22p05_kHz 4
#define Fs_24_kHz 		6
#define Fs_88p2_kHz 	8
#define Fs_768_kHz 	9
#define Fs_96_kHz 		10
#define Fs_176p4_kHz 12
#define Fs_192_kHz 	14

static int get_audio_fs(void)
{
	unsigned value, value1;
	unsigned char config_extend;
	
	sii9293_HAL_SetChipAddr(Page[1]);
	value = sii9293_HAL_ReadByte(TCLK_FS);
	value = value & 0x0f;
	printf("\n**%s\n",__func__);
	printf("Fs : %x\n", value);
	switch (value)
	{
		case Fs_44p1_kHz:
			printf("44.1k\n");
			value1 = 44100;
			config_extend = 1;
			break;

		case Fs_48_kHz:
			printf("48k\n");
			value1 = 48000;
			config_extend = 1;
			break;

		case Fs_32_kHz:
			printf("32k\n");
			value1 = 32000;
			config_extend = 1;
			break;
		
		case Fs_22p05_kHz:
			printf("22.05k\n");
			value1 = 44100;
			config_extend = 1;
			break;
		
		case Fs_24_kHz:
			printf("24k\n");
			value1 = 44100;
			config_extend = 1;
			break;
		
		case Fs_88p2_kHz:
			printf("88.2k\n");
			value1 = 44100;
			config_extend = 1;
			break;
		
		case Fs_768_kHz:
			printf("768k\n");
			value1 = 44100;
			config_extend = 1;
			break;
		
		case Fs_96_kHz:
			printf("96k\n");
			value1 = 44100;
			config_extend = 1;
			break;
		
		case Fs_176p4_kHz:
			printf("176.4k\n");
			value1 = 44100;
			config_extend = 1;
			break;
		
		case Fs_192_kHz:
			printf("192k\n");
			value1 = 44100;
			config_extend = 1;
			break;

		default:
			printf("unsupport audio fs\n");
			break;
	}
	
	return value1;
}


static int audio_turnOn(audio_info_s *audio_info )
{
	int value, value1;
	unsigned char config_extend;

	sii9293_HAL_SetChipAddr(Page[1]);
	//value = sii9293_HAL_ReadByte(TCLK_FS);
	//value1 = value & 0x0f;
	value = get_audio_fs();
	printf("\n**%s\n",__func__);
	printf("Fs : %x\n", value);
	switch(value)
	{
		case 44100:
			printf("44.1k\n");
			audio_info->fs = 44100;
			config_extend = 1;
			break;

		case 48000:
			printf("48k\n");
			audio_info->fs = 48000;
			config_extend = 1;
			break;

		case 32000:
			printf("32k\n");
			audio_info->fs = 32000;
			config_extend = 1;
			break;
			//FIXME

		default:
			printf("unsupport audio fs\n");
			break;
	}
	
	sii9293_HAL_SetChipAddr(Page[1]);
	value = sii9293_HAL_ReadByte(CHST5);
	printf("CHST5 : 0x%x \n", value);
	value &= 0x0f;
	value1 = value & 0x0e;
	value1 = value1 >> 1;
	if (value & 0x01)
	{
		printf("audio length : %x\n", value);
		switch (value1)
		{
			case 0:
				audio_info->audio_bits = 24;
				printf("audio length : 24\n");
				break;
			case 1:
				audio_info->audio_bits = 20;
				printf("audio length : 20\n");
				break;
			case 2:
				audio_info->audio_bits = 22;
				printf("audio length : 22\n");
				break;
			case 4:
				audio_info->audio_bits = 23;
				printf("audio length : 23\n");
				break;
			case 5:
				audio_info->audio_bits = 24;
				printf("audio length : 24\n");
				break;
			case 6:
				audio_info->audio_bits = 21;
				printf("audio length : 21\n");
				break;
			default:
				printf("unsupport audio length\n");
				break;
		}
	}
	else
	{
		switch (value1)
		{
			case 0:
				audio_info->audio_bits = 20;
				printf("audio length : 20\n");
				break;
			case 1:
				audio_info->audio_bits = 16;
				printf("audio length : 16\n");
				break;
			case 2:
				audio_info->audio_bits = 18;
				printf("audio length : 18\n");
				break;
			case 4:
				audio_info->audio_bits = 19;
				printf("audio length : 19\n");
				break;
			case 5:
				audio_info->audio_bits = 20;
				printf("audio length : 20\n");
				break;
			case 6:
				audio_info->audio_bits = 17;
				printf("audio length : 17\n");
				break;
			default:
				printf("unsupport audio length\n");
				break;
		}
	}
}


/*****************************************************************************/
/**
 *  @brief	
 *
 *  @return	aduio mute status
 *  @retval	1 : Unmute
 *  @retval	0 : Mute
 *
 *****************************************************************************/

int Is_AVMute_clear(void)
{
	unsigned char value1;

	//sii9293_HAL_SetChipAddr(Page[1]); //0x68
	//value1 = sii9293_HAL_ReadByte(AUDO_MUTE);
	sii9293_HAL_SetChipAddr(Page[0]); //0x64
	value1 = sii9293_HAL_ReadByte(INTR3);
	
	if(value1 & BIT6) //0:
		return 1; //avmute is clear
	else
		return 0; //Mute
}

/*****************************************************************************/
/**
 *  @brief	 Determine if audio mode changes
 *
 *  @return	Status
 *  @retval	1 : Detects audio mode Change
 *  @retval	0 : Does not detect the audio mode Change
 *
 *****************************************************************************/
int Is_audioMode_change(audio_info_s *audio_info)
{
	unsigned char value;
	unsigned char value1;
	unsigned int tmp = 0;
	
	value = get_audio_fs();
	if (value != audio_info->fs)
	{
		printf("fs changed\n");
		return 1;
	}
	else
		return 0;
}


int audio_turnOff_output_tri(void)
{
	return 0;
}

int Is_audioAutoMute(void)
{
	return 0;
}

int Is_audioFifoErr(void)
{
	return 0;
}

int Is_audioErr_occur(void)
{
	return 1;
}

/*****************************************************************************/
/**
 *  @brief	Get the HDMI 
 *
 *  @return	Status
 *  @retval	1 : Detects HDMI Change
 *  @retval	0	: Does not detect the HDMI Change
 *
 *****************************************************************************/
int get_video_hdmiModeChange(void)
{
	unsigned int value;
	
	//sii9293_HAL_SetChipAddr(Page[1]);
	//value = sii9293_HAL_ReadByte(AUDP_STAT); //HDMI Mode
	//printf("AUDP_STAT : %x \n", value);
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(INTR2);
	printf("INTR2 : %x \n", value);
	if (value & 0x80)
		return 1;
	else
		return 0;
}

/*****************************************************************************/
/**
 *  @brief	Get the HDMI
 *
 *  @return	Status
 *  @retval	1 : Detects HDMI
 *  @retval	0	: Does not detect the HDMI
 *
 *****************************************************************************/
int get_video_hdmiMode(void)
{
	unsigned int value;
	
	//sii9293_HAL_SetChipAddr(Page[1]);
	//value = sii9293_HAL_ReadByte(AUDP_STAT); //HDMI Mode
	//printf("AUDP_STAT : %x \n", value);
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(INTR2);
	printf("INTR2 : %x \n", value);
	if (value & 0x80)
		return 1;
	else
		return 0;
}

/*****************************************************************************/
/**
 *  @brief	get video interlace mode
 *
 *  @return	Status
 *  @retval	1 : Input interlaced
 *  @retval	0	: Input progressive
 *
 *****************************************************************************/
int get_video_interlaceMode(void)
{
	unsigned int value = 0;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(VID_STAT); //Set if hardware detects interlaced video source.
	//printf("VID_STAT : %x \n", value);
	if (value & BIT2)
		return 1;	// Input interlaced
	else
		return 0;	// Input progressive	
}

/*****************************************************************************/
/**
 *  @brief	get video total width
 *
 *  @return	total width 
 *
 *****************************************************************************/
int get_video_total_width(void)
{
	unsigned int value1, value2;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value1 = sii9293_HAL_ReadByte(H_RESL);	
	value2 = sii9293_HAL_ReadByte(H_RESH);
	//printf("H_RESL : %x \n", H_RESL);
	//printf("H_RESH : %x \n", H_RESH);
	return ((value2 & 0x1f) << 8) | value1;
}

/*****************************************************************************/
/**
 *  @brief	get video total height
 *
 *  @return	total height 
 *
 *****************************************************************************/
int get_video_total_height(void)
{
	unsigned int value1, value2;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value1 = sii9293_HAL_ReadByte(V_RESL);	
	value2 = sii9293_HAL_ReadByte(V_RESH);	//
	
	return ((value2 & 0x1f) << 8) | value1;
}

int get_video_pix_VTAVL(void)
{
	unsigned int value1;

	sii9293_HAL_SetChipAddr(Page[0]);
	value1 = sii9293_HAL_ReadByte(VID_VTAVL);	//Video DE Line Registers
	printf("VID_VTAVL : %d \n", value1 & 0x7f);
	return value1 & 0x7f;
}

int get_video_pix_VFP(void)
{
	unsigned int value1;

	sii9293_HAL_SetChipAddr(Page[0]);
	value1 = sii9293_HAL_ReadByte(VID_VFP);	//Video DE Line Registers
	printf("VID_VFP : %d \n", value1 & 0x3f);
	return value1 & 0x3f;
}

int get_video_HFP(void)
{
	unsigned int value1, value2;
	
	sii9293_HAL_SetChipAddr(Page[0]);

	
	value1 = sii9293_HAL_ReadByte(VID_HFP1);	//Video DE Line Registers
	value2 = sii9293_HAL_ReadByte(VID_HFP2);	//

	printf("VID_HFP : %d \n", ((value2 & 0x7) << 8) | value1);
	return ((value2 & 0x7) << 8) | value1;
}

int get_video_HSWIDTH(void)
{
	unsigned int value1, value2;
	
	sii9293_HAL_SetChipAddr(Page[0]);

	
	value1 = sii9293_HAL_ReadByte(VID_HSWID1);	//Video DE Line Registers
	value2 = sii9293_HAL_ReadByte(VID_HSWID2);	//

	printf("HSWIDTH : %d \n", ((value2 & 0x3) << 8) | value1);

	return ((value2 & 0x3) << 8) | value1;
}

/*****************************************************************************/
/**
 *  @brief	get video frameRate
 *
 *  @return	frame Rate value
 *
 *****************************************************************************/
int get_video_frameRate(void)
{
	unsigned int total_width;
	
	total_width = get_video_total_width();
	//printf("total_width : %d \n", total_width);
	if (get_video_interlaceMode())
	{
		if((2640 == total_width) || (1980 == total_width) || (864 == total_width))
			return 25;
		else if((2200 == total_width) || (858 == total_width))
			return 30;
		else
			printf("unsupport total_width \n");
	}
	else
	{
		if((2640 == total_width) || (1980 == total_width) || (864 == total_width))
			return 50;
		else if((2200 == total_width) || (858 == total_width) || (1650 == total_width) || (1904 == total_width)|| (1864 == total_width)|| (1792 == total_width)|| (1688 == total_width) || (1800 == total_width)  || (1680 == total_width) )
			return 60;
		else if(2750 == total_width) 
			return 24;
		else
			return 60;
	}
}

/*****************************************************************************/
/**
 *  @brief	get video pix width
 *
 *  @return	pix width value
 *
 *****************************************************************************/
int get_video_pix_width(void)
{
	unsigned int value1, value2;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value1 = sii9293_HAL_ReadByte(DE_PIX1);	//Video DE Pixels Registers
	value2 = sii9293_HAL_ReadByte(DE_PIX2);	
	//printf("DE_PIX1 : %x \n", value1);
	//printf("DE_PIX2 : %x \n", value2);
	
	return ((value2 & 0x1f) << 8) | value1;
}

/*****************************************************************************/
/**
 *  @brief	get video pix hight
 *
 *  @return	pix height value
 *
 *****************************************************************************/
int get_video_pix_height(void)
{
	unsigned int value1, value2;
	
	sii9293_HAL_SetChipAddr(Page[0]);
#if 0
	value1 = sii9293_HAL_ReadByte(VID_VTAVL);	//Video DE Line Registers
	printf("VID_VTAVL : %d \n", value1 & 0x7f);

	value1 = sii9293_HAL_ReadByte(VID_VFP);	//Video DE Line Registers
	printf("VID_VFP : %d \n", value1 & 0x3f);

	value1 = sii9293_HAL_ReadByte(VID_HFP1);	//Video DE Line Registers
	value2 = sii9293_HAL_ReadByte(VID_HFP2);	//

	printf("VID_HFP : %d \n", ((value2 & 0x7) << 8) | value1);

	value1 = sii9293_HAL_ReadByte(VID_HSWID1);	//Video DE Line Registers
	value2 = sii9293_HAL_ReadByte(VID_HSWID2);	//

	printf("HSWIDTH : %d \n", ((value2 & 0x3) << 8) | value1);
#endif



	value1 = sii9293_HAL_ReadByte(DE_LINL);	//Video DE Line Registers
	value2 = sii9293_HAL_ReadByte(DE_LINH);	//
	//printf("DE_LINL : %x \n", value1);
	//printf("DE_LINH : %x \n", value2);
	
	return ((value2 & 0x1f) << 8) | value1;
}

/*****************************************************************************/
/**
 *  @brief	get video avmute, Status of the Hardware Soft Mute
 *
 *  @return	Status
 *  @retval	1 : Output of mute is 0
 *  @retval	0	: 
 *
 *****************************************************************************/
int get_video_avmute(void)
{
	unsigned int value;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(AEC0_CTRL);
	if (value & BIT3)
		return 1;
	else
		return 0;
}

/*****************************************************************************/
/**
 *  @brief	get video input vsync polarity ,Set if hardware detects interlaced video source.
 *
 *  @return	Status
 *  @retval	1 : Positive polarity (leading edge rises)
 *  @retval	0	: Negative polarity (leading edge falls)
 *
 *****************************************************************************/
int get_video_input_vsync_polarity(void)
{
	unsigned int value;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(VID_STAT); //Set if hardware detects interlaced video source.
	if (value & BIT1)
		return 1; //Positive polarity (leading edge rises)
	else
		return 0; //Negative polarity (leading edge falls)
}


/*****************************************************************************/
/**
 *  @brief	get video input hsync polarity, Set if hardware detects interlaced video source.
 *
 *  @return	Status
 *  @retval	1 : Positive polarity (leading edge rises)
 *  @retval	0	: Negative polarity (leading edge falls)
 *
 *****************************************************************************/
int get_video_input_hsync_polarity(void)
{
	unsigned int value;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	value = sii9293_HAL_ReadByte(VID_STAT); //Set if hardware detects interlaced video source.
	if (value & BIT0)
		return 1; //Positive polarity (leading edge rises)
	else
		return 0; //Negative polarity (leading edge falls)
}


/*****************************************************************************/
/**
 *  @brief		
 *
 *  @return	Status
 *  @retval	1 : 
 *  @retval	0	: 
 *
 *****************************************************************************/
int video_path_set(video_info_s *video_info)
{
	if (video_info->hdmiMode)
	{
		video_info->hdmi_mode = YUV422;
	}

	return 0;
}

/*****************************************************************************/
/**
 *  @brief	Detect whether the video source format changes	
 *
 *  @return	Status
 *  @retval	1 : 
 *  @retval	0	: 
 *
 *****************************************************************************/
int check_video_mode_change(video_info_s *video_info)
{
	unsigned char value;

	sii9293_HAL_SetChipAddr(Page[0]);

	value = sii9293_HAL_ReadByte(INTR2_MASK);
	sii9293_HAL_WriteByte(INTR2_MASK, value|BIT0); //enable  Video clock frequency change detected
	value = sii9293_HAL_ReadByte(INTR2);

	if (value & BIT0)
	{
		printf("mode change,INTR2 :%x \n",value);
		value = sii9293_HAL_ReadByte(INTR2);
		sii9293_HAL_WriteByte(INTR2, value); //clean Video clock frequency change detected



	}


	value = sii9293_HAL_ReadByte(INTR5);

	if (value & (BIT3 | BIT4))
	{
		printf("mode change,INTR5 :%x \n",value);
		value = sii9293_HAL_ReadByte(INTR5);
		sii9293_HAL_WriteByte(INTR5, value); //clean Video clock frequency change detected



	}

	//video_info->hdmi_mode = YUV422;

	return 0;
}

int check_video_format_change(video_info_s *video_info)
{
	static int video_width_new = 0;
	static int video_height_new = 0;
	static int video_rate_new = 0;
	//static int video_width_tmp = 0;
	//static int video_height_tmp = 0;
	int tmp = 0;
	
	video_width_new = get_video_pix_width();
	video_height_new = get_video_pix_height();
	video_rate_new = get_video_frameRate();
	//printf("video_width_new = %d \n", video_width_new);
	//printf("video_height_new = %d \n", video_height_new);
	//printf("video_info->width = %d \n", video_info->width);
	//printf("video_info->height = %d \n", video_info->height);
	
	//while ((video_info->width != video_width_new) \
		|| (video_info->height != video_height_new) \
		|| (video_info->frameRate != video_rate_new))
	while ((video_info->height != video_height_new))
	{
		if (1080 == video_height_new || \
			720 == video_height_new || \
			540 == video_height_new || \
			1050 == video_height_new || \
			900 == video_height_new || \
			768 == video_height_new || \
			1024 == video_height_new || \
			800 == video_height_new || \
			960 == video_height_new
			)
		{
			printf("video_width_new = %d \n", video_width_new);
			printf("video_height_new = %d \n", video_height_new);
			printf("video_info->width = %d \n", video_info->width);
			printf("video_info->height = %d \n", video_info->height);
			
			video_info->width = video_width_new;
			video_info->height = video_height_new;

			return 1;
		}
		
		break;
	}
	
	return 0;
}

int Is_set_mute(void)
{
	int value = 0;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	
	value = sii9293_HAL_ReadByte(INTR3);
	printf("value = %d \n", value);
	value &= BIT6;
	printf("value = %d \n", value);
	if (BIT6 == value)
	{
		sii9293_HAL_WriteByte(INTR3, BIT6); //clean 
		return 1;
	}
	else
		return 0;	
}

/*****************************************************************************/
/**
 *  @brief	Detecting if there is any video source inputs, 
 *					and format of the input video source information.	
 *
 *  @return	Status
 *  @retval	1 : 
 *  @retval	0	: 
 *
 *****************************************************************************/
int check_video_timing(video_info_s *video_info)
{
	unsigned int value = 0, total_height = 0;
	unsigned int total_width = 0;
	int count = 0;


RX_interrupt:

	printf("check_video_timing \n");

	sii9293_HAL_SetChipAddr(Page[0]);
	//	value = sii9293_HAL_ReadByte(INTR_STATE);	
	//	printf("INTR_STATE : ", value);
	//	value = sii9293_HAL_ReadByte(RX_INTR_STATE_2); //check Receiver Side Interrupt
	//	printf("RX_INTR_STATE_2 : ", value);
	value = sii9293_HAL_ReadByte(STATE);	//hdmi in: 0x13 ,hdmi out: 0x10
	printf("STATE : %x\n", value);
#if 1
	//if (!(value &= 0x01))
	if (!(value &= 0x02))
	{
		count++;

		if (10 == count)
		{
			printf("Does not detect the video source \n");
			return 0;
		}

		goto RX_interrupt;
	}
#endif
#if 1
	//HDMI MODE
	//video_info->hdmiMode  = get_video_hdmiModeChange();
	video_info->hdmiMode = get_video_hdmiMode(); 
	printf("video_info->hdmiMode :%d\n", video_info->hdmiMode);	

	//interlaceMode
	video_info->interlaceMode = get_video_interlaceMode();	
	printf("video_info->interlaceMode :%d\n", video_info->interlaceMode);

	//frameRate
	video_info->frameRate = get_video_frameRate();
	printf("video_info->frameRate :%d\n", video_info->frameRate);
#if 1
	//total_width
	total_width = get_video_total_width();
	printf("total_width :%d\n", total_width);

	//total_height
	total_height = get_video_total_height();
	printf("total_height :%d\n", total_height);
#endif
	//pix width
	video_info->width = get_video_pix_width();
	printf("video_info->width :%d\n", video_info->width);

	//pix height
#if 0
	video_info->height = get_video_pix_height();
	printf("video_info->height :%d\n", video_info->height);
#else
	video_info->vtavl = get_video_pix_VTAVL();
	video_info->height = get_video_pix_VTAVL();
	video_info->vfp = get_video_pix_VFP();

	video_info->height = total_height - video_info->vtavl - video_info->vfp;

	printf("video_info->height :%d\n", video_info->height);

#endif
	//avmute
	video_info->avmute = get_video_avmute();
	printf("video_info->avmute :%d\n", video_info->avmute);

	//input_vsync_polarity
	video_info->input_vsync_polarity = get_video_input_vsync_polarity();
	printf("video_info->input_vsync_polarity :%d\n", video_info->input_vsync_polarity);

	//input_hsync_polarity
	video_info->input_hsync_polarity = get_video_input_hsync_polarity();
	printf("video_info->input_hsync_polarity :%d\n", video_info->input_hsync_polarity);
#endif
	return 1;
}


void sii9293_Audio_clock(void)
{
	int state;
	
	sii9293_HAL_SetChipAddr(Page[0]);
	state = sii9293_HAL_ReadByte(STATE) & BIT1;

	if (0x02 == state)
	{
		sii9293_HAL_ReadByte(INTR3);
		sii9293_HAL_ReadByte(N_HVAL1);
		sii9293_HAL_ReadByte(N_HVAL2);
		sii9293_HAL_ReadByte(N_HVAL3);
		sii9293_HAL_ReadByte(CTS_HVAL1);
		sii9293_HAL_ReadByte(CTS_HVAL2);
		sii9293_HAL_ReadByte(CTS_HVAL3);
		sii9293_HAL_WriteByte(AUDP_MUTE, 0x00); //Polarity of the MUTE_OUT:Positive
		
		sii9293_HAL_WriteByte(ACR_CTRL1, BIT0); //NCLK edge , Set this bit to 1 to generate the strobe
		sii9293_HAL_WriteByte(I2S_CTRL1, BIT6);	//Sample edge is falling (negative)
		sii9293_HAL_WriteByte(I2S_CTRL2, BIT4|BIT3); //0x18 Channel enabled The SiI9293A uses only SD0.& Enable MCLKOUT
		sii9293_HAL_WriteByte(INTR2, BIT3|BIT1);	//0x06
		sii9293_HAL_WriteByte(INTR4, BIT3|BIT2|BIT1|BIT0); //0x0F
		sii9293_HAL_WriteByte(INTR5, BIT7|BIT6|BIT5|BIT0);	//0xE1
		sii9293_HAL_WriteByte(INTR5_MASK, BIT6); //0x40
		sii9293_HAL_WriteByte(AEC0_CTRL, BIT2|BIT0); //0x05
		sii9293_HAL_WriteByte(AUDP_MUTE, 0x00);
		state = sii9293_HAL_ReadByte(INTR3);
		if (state & 0x80)
		{
			sii9293_HAL_WriteByte(INTR3, BIT7|BIT5|BIT2|BIT0); //0xA5
			sii9293_HAL_ReadByte(AUDIO_TYPE);
			sii9293_HAL_ReadByte(AUDIO_VERS);
			sii9293_HAL_ReadByte(AUDIO_LEN);
			sii9293_HAL_ReadByte(AUDIO_CHSUM);
			sii9293_HAL_ReadByte(AUDIO_DBYTE1);
			sii9293_HAL_ReadByte(AUDIO_DBYTE2);
			sii9293_HAL_ReadByte(AUDIO_DBYTE3);
			sii9293_HAL_ReadByte(AUDIO_DBYTE4);
			sii9293_HAL_ReadByte(AUDIO_DBYTE5);
			sii9293_HAL_ReadByte(AUDIO_DBYTE6);
			sii9293_HAL_ReadByte(AUDIO_DBYTE7);
			sii9293_HAL_ReadByte(AUDIO_DBYTE8);
			sii9293_HAL_ReadByte(AUDIO_DBYTE9);
			sii9293_HAL_ReadByte(AUDIO_DBYTE10);
			
			sii9293_HAL_ReadByte(AUDIO_DBYTE4);
		}
	}
}


/*
	****video
*/




/*
	******
*/
void sii9293_test(void)
{
	int value;
//	sii9293_HAL_SetChipAddr(Page[0]);
//	sii9293_HAL_ReadByte(INTR2);
//	sii9293_HAL_ReadByte(STATE);
//	sii9293_HAL_ReadByte(VID_CTRL);
//	sii9293_HAL_ReadByte(VID_STAT);
		/*
			***interrupt infomation
		*/
#if 0
	sii9293_HAL_SetChipAddr(Page[0]);
	sii9293_HAL_ReadByte(INTR_STATE);
	sii9293_HAL_ReadByte(INTR1);
	sii9293_HAL_ReadByte(INTR2);
	sii9293_HAL_ReadByte(INTR3);
	sii9293_HAL_ReadByte(INTR4);
	sii9293_HAL_ReadByte(INTR5);
	sii9293_HAL_ReadByte(INTR6);
	sii9293_HAL_ReadByte(RX_INTR7);
	sii9293_HAL_ReadByte(RX_INTR8);
#endif
	/*
		**video infomation 
	*/
#if 0
	sii9293_HAL_SetChipAddr(Page[0]);
	printf("Hsync : %d \n", sii9293_HAL_ReadByte(H_RESL));	//Hsync time
	printf("%d \n", sii9293_HAL_ReadByte(V_RESL));
	printf("%d \n", sii9293_HAL_ReadByte(DE_PIX1));
	printf("%d \n", sii9293_HAL_ReadByte(DE_PIX2));
	printf("%d \n", );
	printf("%d \n", );
	printf("%d \n", );
	printf("%d \n", );
	printf("%d \n", );
	printf("%d \n", );
	printf("%d \n", );
	printf("%d \n", );
	//sii9293_HAL_ReadByte(H_RESH);
	sii9293_HAL_ReadByte(V_RESL);	//Vsync time
	sii9293_HAL_ReadByte(V_RESH);
	sii9293_HAL_ReadByte(DE_PIX1);	//Video DE Pixels Registers
	sii9293_HAL_ReadByte(DE_PIX2);	//
	sii9293_HAL_ReadByte(DE_LINL);	//Video DE Line Registers
	sii9293_HAL_ReadByte(DE_LINH);	//
	sii9293_HAL_ReadByte(VID_VTAVL);
	sii9293_HAL_ReadByte(VID_VFP);
	sii9293_HAL_ReadByte(VID_STAT);
	sii9293_HAL_ReadByte(VID_HFP1);
	sii9293_HAL_ReadByte(VID_HFP2);
	sii9293_HAL_ReadByte(VID_HSWID1);
	sii9293_HAL_ReadByte(VID_HSWID2);
	sii9293_HAL_ReadByte(VID_XPCNT1);
	sii9293_HAL_ReadByte(VID_XPCNT2);
#endif
#if 0 //video 
	
	video_info_s video_info;
	audio_info_s audio_info;
	check_video_timing(&video_info);
	Is_HDMI_5V_detected();
	audio_turnOn(&audio_info);
	
	sii9293_HAL_SetChipAddr(Page[1]);
	value = sii9293_HAL_ReadByte(AUDP_STAT); //HDMI Mode
	
	printf("video_info->hdmiMode :%d\n", value);	
	
	printf("Is_HDMI_5V_detected  :%d\n", Is_HDMI_5V_detected());
	printf("Is_Sync_stable  :%d\n", Is_Sync_stable());
	printf("Is_eccErr_occur  :%d\n", Is_eccErr_occur());
	value = sii9293_HAL_ReadByte(VID_AOF);
	printf("VID_AOF : %d\n", value);
	

	printf("hdmiMde : %d \n", get_video_hdmiMode());
	printf("interlaceMode : %d \n",get_video_interlaceMode());
	printf("frameRate : %d \n",get_video_frameRate());
	printf("height : %d \n",get_video_total_height());
	printf("pix_width : %d \n",get_video_pix_width());
	printf("pix_height : %d \n",get_video_pix_height());
	printf("vsync : %d \n",get_video_input_vsync_polarity());
	printf("hsync : %d \n",get_video_input_hsync_polarity());
	printf("Is_HDMI_5V_detected  :%d\n", Is_HDMI_5V_detected());
	printf("Is_Sync_stable  :%d\n", Is_Sync_stable());
#endif
#if 1 //audio config
	
	Audio_init();
	
#endif
	
#if 0
	sii9293_HAL_WriteByte(0xff, 0x80);
	sii9293_HAL_WriteByte(0xee, 0x01);
	printf("0xff = 0x%x \n", sii9293_HAL_ReadByte(0x00));
	
	sii9293_HAL_WriteByte(0xff, 0x82);
	printf("0x7c = 0x%x \n", sii9293_HAL_ReadByte(0x7c));
	printf("0x7a = 0x%x \n", sii9293_HAL_ReadByte(0x7a));
	
	printf("0x80 = 0x%x \n", sii9293_HAL_ReadByte(0x80));
	printf("0x7e = 0x%x \n", sii9293_HAL_ReadByte(0x7e));
	
	printf("0x78 = 0x%x \n", sii9293_HAL_ReadByte(0x78));
	printf("0x72 = 0x%x \n", sii9293_HAL_ReadByte(0x72));
	printf("0x76 = 0x%x \n", sii9293_HAL_ReadByte(0x76));
	
	printf("0x75 = 0x%x \n", sii9293_HAL_ReadByte(0x75));
	printf("0x71 = 0x%x \n", sii9293_HAL_ReadByte(0x71));
	printf("0x74 = 0x%x \n", sii9293_HAL_ReadByte(0x74));
	
	//printf("0x7e = 0x%x \n", sii9293_HAL_ReadByte(0x7e));
	//printf("0x7e = 0x%x \n", sii9293_HAL_ReadByte(0x7e));
#endif

#if 1
	
#endif

}

#if 1
int chip_init(void)
{
	int ret;

	ret = i2c_init();
	if(ret)
		return -1;


	chip = malloc(sizeof(sii9293_chip_s));
	if (!chip)
	{
		printf("malloc failed\n");
		return -1;
	}
	memset(chip, 0x00,sizeof(sii9293_chip_s));

	chip->video_status = Unplug; //TODO bly

	return 0;
}
#endif
//#define AUDIO_OUTPUT
#ifdef AUDIO_OUTPUT
#if 1
static int oss_set_rate(int fd, int rate)
{
	int arg = 0;
	int status = -1;

	arg = rate;
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);
	if(status == -1){
		printf("SOUND_PCM_WRITE_RATE ioctl failed\n");
	}
	
	if(rate != arg){
		printf("unable to set sample size\n");
		return -1;
	}
	return 0;
}

static int oss_set_channel(int fd, int channel)
{
	int status = -1;
	int arg;

	arg = channel;
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);
	if(status == -1){
		printf("SOUND_PCM_WRITE_CHANNELS ioctl failed\n");
		return -1;
	}
	
	if(arg != channel){
		printf("unable to set number of channels(%d, %d)\n", arg, channel);
		return -1;
	}
	return 0;
}

static int oss_set_bit(int fd, int bit)
{
	int arg = 0;
	int status = -1;

	arg = bit;
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg);
	if(status == -1){
		printf("SOUND_PCM_WRITE_BITS ioctl failed\n");
		return -1;
	}

	if(arg != bit){
		printf("unable to set sample size\n");
		return -1;
	}

	return 0;
}
#endif
#endif
int  audio_config(void)
{
	audio_info_s *audio_info;
	audio_info = &(chip->audio_info);
	int tmp;

	printf("\n\n***audio_config\n");
	//#ifdef LINUX_SOUND
#if 0
	if(rdfd_dsp < 0)
	{
		rdfd_dsp=open("/dev/dsp", O_RDONLY);
		if(rdfd_dsp<0){
			printf("fail to open /dev/dsp\n");
			return -1;
		}
	}
	//if(16 == audio_info->audio_bits) //FIXME
	tmp = AFMT_S16_NE;
	if (ioctl(rdfd_dsp, SNDCTL_DSP_SETFMT, &tmp) == -1 || tmp != AFMT_S16_NE) {
		close(rdfd_dsp);
		return -1;
	}  
	tmp = audio_info->fs;//WAVE_SAMPLE_RATE;

	if (ioctl(rdfd_dsp, SNDCTL_DSP_SPEED, &tmp) == -1) {
		close(rdfd_dsp);
		return -1;
	}
#ifdef AUDIO_OUTPUT
	if(fd_w < 0)
	{
		fd_w = open("/dev/dsp", O_WRONLY);
		if (fd_w < 0) {
			printf("fail to open write /dev/dsp\n");
			return -1;
		}
	}
	if (oss_set_channel(fd_w, 2))//FIXME
		return;
	if (oss_set_rate(fd_w, audio_info->fs))
		return;
	if (oss_set_bit(fd_w, AFMT_S16_LE))
		return ;
#endif
	//if(2 == audio_info->chns) //FIXME chns config
#else
	if(audio_fd < 0)
	{
		audio_fd=open("/dev/silan-i2s", O_RDWR);
		if(audio_fd<0){
			printf("fail to open /dev/silan-i2s\n");
			return -1;
		}
		if (ioctl(audio_fd, AUDIO_IOCTL_GET_OUT_START_DMA_ADDR, &tmp) == -1) {
			close(audio_fd);
			return -1;
		}
		audio_dma_out.start_addr = tmp;
		audio_dma_out.wrPtr = audio_dma_out.rdPtr = audio_dma_out.start_addr;

		if (ioctl(audio_fd, AUDIO_IOCTL_GET_OUT_DMA_SIZE, &tmp) == -1) {
			close(audio_fd);
			return -1;
		}
		audio_dma_out.size = tmp;

		//audio_dma_in.start_addr = audio_dma_out.start_addr + audio_dma_out.size;
		audio_dma_in.start_addr = audio_dma_out.start_addr;
		audio_dma_in.size = audio_dma_out.size;
		audio_dma_in.wrPtr = audio_dma_in.rdPtr = audio_dma_in.start_addr;

		tmp =  (unsigned long)mmap(NULL, audio_dma_in.size*2, PROT_READ | PROT_WRITE, MAP_SHARED, audio_fd, 0);

		audio_dma_out.start_addr_va = tmp;
		//audio_dma_in.start_addr_va = audio_dma_out.start_addr_va +  audio_dma_out.size ;
		audio_dma_in.start_addr_va = audio_dma_out.start_addr_va;
	}
	//FIXME
	audio_dma_out.wrPtr = audio_dma_out.rdPtr = audio_dma_out.start_addr;
	audio_dma_in.wrPtr = audio_dma_in.rdPtr = audio_dma_in.start_addr;

	//printf("audio_dma_out.wrPtr:%x\n",audio_dma_out.wrPtr);
	//printf("audio_dma_in.wrPtr:%x\n",audio_dma_in.wrPtr);

	audio_configed = 0;
	if(audioIn_trigger) {
		//trigger
		if (ioctl(audio_fd, AUDIO_IOCTL_IN_TRIGGER_OFF, &tmp) == -1) {

			close(audio_fd);
			return -1;
		}
		audioIn_trigger = 0;
	}

	if(audioOut_trigger) {
		//trigger
		if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER_OFF, &tmp) == -1) {

			close(audio_fd);
			return -1;
		}
		audioOut_trigger = 0;
	}
	tmp = 2; //FIXME
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_OUT_CHNS, &tmp) == -1) {
		close(audio_fd);
		return -1;

	}

	tmp = 16; //FIXME
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_OUT_BITS, &tmp) == -1) {
		close(audio_fd);
		return -1;

	}

	tmp = audio_info->fs;
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_OUT_FS, &tmp) == -1) {
		close(audio_fd);
		return -1;

	}

	tmp = 2; //FIXME
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_IN_CHNS, &tmp) == -1) {
		close(audio_fd);
		return -1;

	}

	tmp = 16; //FIXME
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_IN_BITS, &tmp) == -1) {
		close(audio_fd);
		return -1;

	}

	tmp = audio_info->fs;
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_IN_FS, &tmp) == -1) {
		close(audio_fd);
		return -1;
	}

	audio_configed = 1;
#endif
	return 0;

}

#if 1

void *sii9293_handler(void *p)
{
	video_info_s *video_info;
	audio_info_s *audio_info;
	int ret = -1, timeOutReset_5V = 0,timeoutWaitSync=0;
	unsigned int tmp = 0;
	extern volatile int viu_started;
	extern volatile int viu_configed;
	
	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );
	chip_init();
	//sleep(3); //yuliubing FIXME
	sii9293_init_register();
#if 1 //audio config
	Audio_init();
#endif
	KVM_REST();
	video_info = &(chip->video_info);
	audio_info = &(chip->audio_info);
	//check_video_timing(video_info); //FIXME
	
	sleep(1);
	audio_info->audio_bits = 16;
	audio_info->fs = 48000;
	//sii9293_Audio_clock();
#if 0 //auido
	//sii9293_test();
	chip->g_audio_output = 1;
	audio_info->fs = 48000;
	audio_info->chns = 2;
	h264_append_info.fs = audio_info->fs;
	//chip->audio_status = AudioOn;
	//chip->audio_prev_status = AudioOn;
	
	//audio_info->audio_bits = 16;
	audio_config();
	h264_append_info.audio_bits = audio_info->audio_bits;
	h264_append_info.chns = audio_info->chns;
	
	rtsp_audio_config(audio_info->fs, audio_info->audio_bits, audio_info->chns); //defined from live555
#endif

	while (1)
	{
		switch (chip->video_status)
		{
			case Unplug:	//unplug
				printf("Unplug \n");
#ifdef APP_CODE
				gbBandwidthDetectMode = SL_TRUE;
				HDMI_light_off();
#endif
				HDMI_lost = 1;
				timeOutReset_5V++;
				if (timeOutReset_5V > 120)
				{
					reboot1();
				}
				
				if (audioOut_trigger)
				{
					//trigger
					if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER_OFF, &tmp) == -1) 
					{
						close(audio_fd);
						return NULL;
					}
					printf("audio out trigger off\n");
					audioOut_trigger = 0;
				}
				audio_configed = 0;	
				if(Is_HDMI_5V_detected())
					chip->video_status = WaitSync;
				break;
				
			case WaitSync:
				printf("WaitSync \n");
				//sleep(3);
				timeoutWaitSync++;
				if(timeoutWaitSync>15)
				{
					/*Win10 first boot, unable to plot, re-plug*/
					HDMI_HotPlug();
					timeoutWaitSync=0;
					printf("waitsync timeout \n");
				}
				if(Is_Sync_stable())
					chip->video_status = CheckSync;
				else if(!Is_HDMI_5V_detected())
					chip->video_status = SWReset;
				break;
			
			case CheckSync:
				printf("CheckSync \n");
				if(!Is_Sync_stable())
					chip->video_status = WaitSync;
				else if(!Is_HDMI_5V_detected())
					chip->video_status = SWReset;
				else
					chip->video_status = ModeDetect;
				break;
			
			case ModeDetect:
				printf("ModeDetect \n");
				if(!Is_Sync_stable())
					chip->video_status = WaitSync;
				else if(!Is_HDMI_5V_detected())
					chip->video_status = SWReset;
				else 
				{
					ret = check_video_timing(video_info);
					if (ret)
					{
						chip->video_status = VideoOn;
#if 1 //audio
						//FIXME
						
						chip->g_audio_output = 1;
						audio_turnOn(audio_info); //audio fs and audio bits
						//audio_info->fs = 44100;//48000;
						//audio_info->fs = 48000;
						//audio_info->audio_bits = 16;
						printf("audio_info->fs = %d \n", audio_info->fs);
						printf("audio_info->audio_bits = %d \n", audio_info->audio_bits);
						audio_info->chns = 2;
						h264_append_info.fs = audio_info->fs;
						
						chip->audio_status = AudioOn;
						//chip->audio_prev_status = AudioOn;

						
						audio_config();
						h264_append_info.audio_bits = audio_info->audio_bits;
						h264_append_info.chns = audio_info->chns;

						rtsp_audio_config(audio_info->fs, audio_info->audio_bits, audio_info->chns); //defined from live555
#endif
					}
					else
						chip->video_status = SWReset;
				}
				break;
			
			case VideoOn:
				//printf("VideoOn \n");
#ifdef APP_CODE
				gbBandwidthDetectMode = SL_FALSE;
				HDMI_light_on();
#endif
				HDMI_lost = 0;
				timeOutReset_5V = 0;
				timeoutWaitSync=0;
				if( VideoOn == chip->video_prev_status)
				{
#if 1
#if 0
					//FIXME
					if(Is_eccErr_occur())
					{
						printf(":eccErr_occur\n");
						chip->video_status = HDCP_Reset;
						chip->audio_status = AudioOff; //FIXME
						
					}
#endif
					//check video mode change
					//check_video_timing(video_info);
					//check_video_format_change(video_info);
					//Is_set_mute();
#if 1
					if (1 == check_video_format_change(video_info))
					//if (1 == Is_set_mute() && 1 == check_video_format_change(video_info))
					{
						printf("void format change \n");
						chip->video_status = Unplug;
						chip->video_prev_status = Unplug;
						chip->g_video_output = 0;
						printf("chip->g_video_output = 0\n");
					}
#endif
#if 0
					if (check_video_format_change(video_info))
					{
						printf("void format change \n");
						chip->video_status = Unplug;
						chip->video_prev_status = Unplug;
						chip->g_video_output = 0;
						printf("chip->g_video_output = 0\n");
					}
#endif
//bly
#if 0
					if(check_video_mode_change(video_info)) 
					{
						printf("video mode changed\n");
						chip->video_status = Unplug;
						chip->video_prev_status = Unplug;
						chip->g_video_output = 0;
						printf("chip->g_video_output = 0\n");
					}
#endif
#endif
				} 
				else 
				{
					printf(":VideoOn\n");
#if 0
					if(Is_eccErr_occur())
					{
						printf(":eccErr_occur\n");
						chip->video_status = HDCP_Reset;
						chip->audio_status = AudioOff; //FIXME
					}
					else 
#endif
					{
						video_path_set(video_info);
						chip->g_video_output = 1;
						printf("\n**chip->g_video_output = 1\n");
#if 1
						if(video_info->hdmiMode)
						{
							chip->audio_status = AudioReq;
						}
#endif
						chip->video_prev_status = VideoOn;
					}

				}
#if 0
				if(!Is_HDMI_5V_detected())
				{
					printf("5V lost");
					chip->video_status = Unplug;
					chip->g_video_output = 0;
					chip->video_prev_status = Unplug;
					chip->audio_status = AudioOff; //FIXME
				}
#else
				if(!Is_Sync_stable())
				{
					printf("5V lost\n");
					chip->video_status = Unplug;
					chip->g_video_output = 0;
					chip->video_prev_status = Unplug;
					chip->audio_status = AudioOff; //FIXME
					
				}
#endif
				break;
			
			case HDCP_Reset:
				printf(":HDCP_Reset\n");
				if(Is_eccErr_occur())
					chip->video_status = SWReset;
				else  
					chip->video_status = VideoOn;
				break;
			
			case SWReset:
				printf(":SWReset\n");
				//swReset(); //FIXME yuliubing
				chip->video_status = Unplug;
			
			default:
				break;
		}
	
#if 0	
		switch (chip->audio_status)
		{
			case AudioOff:
			case AudioReq:
			case AudioConfirm:
			case AudioOn:
				
			default:
				break;
		}
#endif
		usleep(500000);
	}
}

#endif



void sii9293_Auto_audio_control(void)
{
//Set AAC_OE = 0 to disable AAC_OE (if used).
//Set AAC_EN = 0 to disable AAC.
//Set AACRST = 1 to perform AAC reset.
//Set AACRST = 0.
//Set AAC_EN = 1 to enable AAC.
//Set AAC_OE = 1 to enable AAC_OE (if used)
}



/**************************************************************************/
/*
**			9293 IIC read and write device
*/

int i2c_init(void)
{
	fd = open("/dev/i2c-0",O_RDWR);

	if(fd < 0)
	{
		perror("open /dev/i2c-0 failed\n");
		return -1;
	}
	e2prom_data.nmsgs = 2;
	e2prom_data.msgs = (struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));
	if(!e2prom_data.msgs)
	{
		perror("malloc error\n");
		return -1;
	}

	(e2prom_data.msgs[0]).buf = (unsigned char*)malloc(2);
	if(!(e2prom_data.msgs[0]).buf)
	{
		perror("malloc error\n");
		return -1;
	}

	return 0;
}

void i2c_uinit(void)
{

	free(e2prom_data.msgs[0].buf); 
	free(e2prom_data.msgs); 
	close(fd);

}

#if 0
void * I2C_ReadByte(unsigned int addr, unsigned char regOffset)
{
	//IIC read
	unsigned char  data;
	int ret;
	e2prom_data.nmsgs = 2;
	(e2prom_data.msgs[0]).len = 1;
	(e2prom_data.msgs[0]).addr = addr;
	(e2prom_data.msgs[0]).flags = I2C_M_WR;
	(e2prom_data.msgs[0]).buf[0] =regOffset;
	(e2prom_data.msgs[1]).len = 1;
	(e2prom_data.msgs[1]).addr = addr;
	(e2prom_data.msgs[1]).flags = I2C_M_RD;
	(e2prom_data.msgs[1]).buf = &data;

	ret = ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret < 0)
	{
		perror("ioctl error2");
		return -1;
	}

	printf("leave %s %x\n",__func__,data);
	return data;
}
#endif

#if 1
UINT8 I2C_ReadByte(unsigned int addr, unsigned char regOffset)
{
	//IIC read
	unsigned char  data;
	int ret;
	e2prom_data.nmsgs = 2;
	(e2prom_data.msgs[0]).len = 1;
	(e2prom_data.msgs[0]).addr = addr/2;
	(e2prom_data.msgs[0]).flags = I2C_M_WR;
	(e2prom_data.msgs[0]).buf[0] =regOffset;
	(e2prom_data.msgs[1]).len = 1;
	(e2prom_data.msgs[1]).addr = addr/2;
	(e2prom_data.msgs[1]).flags = I2C_M_RD;
	(e2prom_data.msgs[1]).buf = &data;

	ret = ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret < 0)
	{
		perror("ioctl error2");
		return -1;
	}
	//printf("addr : %x \n", addr);
	//printf("leave %s %x\n",__func__,data);
	return data;
}
#endif
#if 0
int I2C_WriteByte(unsigned int addr, unsigned char regOffset,unsigned char data)
{
	int ret;
	e2prom_data.nmsgs = 1;
	//IIC write
	(e2prom_data.msgs[0]).len = 0x2;
	(e2prom_data.msgs[0]).addr = addr;
	(e2prom_data.msgs[0]).flags = I2C_M_WR;
	(e2prom_data.msgs[0]).buf[0] =regOffset;
	(e2prom_data.msgs[0]).buf[1] =data;

	ret = ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret < 0)
	{
		perror("ioctl error2");
		return -1;
	}

	return 0;
}
#endif

#if 1
int I2C_WriteByte(unsigned int addr, unsigned char regOffset,unsigned char data)
{
	int ret;
	e2prom_data.nmsgs = 1;
	//IIC write
	(e2prom_data.msgs[0]).len = 0x2;
	(e2prom_data.msgs[0]).addr = addr/2;
	(e2prom_data.msgs[0]).flags = I2C_M_WR;
	(e2prom_data.msgs[0]).buf[0] =regOffset;
	(e2prom_data.msgs[0]).buf[1] =data;

	ret = ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret < 0)
	{
		perror("ioctl error2");
		return -1;
	}

	return 0;
}
#endif
static void sii9293_HAL_SetChipAddr(UINT8 u8_address)
{
    sii9293_u8chipAddr = u8_address;
}

static void sii9293_HAL_WriteByte(UINT16 u16_index, UINT8 u8_value)
{
	I2C_WriteByte(sii9293_u8chipAddr, u16_index, u8_value);
}

UINT8 sii9293_HAL_ReadByte(UINT16 u16_index)
{
	//UINT8 * p_data = I2C_ReadByte(sii9293_u8chipAddr, u16_index);
	
	//return *p_data;
    return I2C_ReadByte(sii9293_u8chipAddr, u16_index);
}

static void sii9293_HAL_ModBits(UINT16 u16_index, UINT8 u8_mask, UINT8 u8_value)
{
    UINT8 tmp;
	
    tmp  = sii9293_HAL_ReadByte(u16_index);
    tmp &= (~u8_mask);
    tmp |= (u8_value & u8_mask);
    sii9293_HAL_WriteByte(u16_index, tmp);
}

static void sii9293_HAL_SetBits(UINT16 u16_index, UINT8 u8_mask)
{
    sii9293_HAL_ModBits(u16_index, u8_mask, u8_mask);
}

static void sii9293_HAL_ClrBits(UINT16 u16_index, UINT8 u8_mask)
{
    sii9293_HAL_ModBits(u16_index, u8_mask, 0x00);
}


