/* * Note: In this example, the source video data is read from a YUV file via MDEV
 *       write, and the BSG result is dumped to a file via MDEV read.
 */
#include "../version.h"
#include <sys/types.h>
#include <sys/socket.h>                         // 包含套接字函数库
#include <stdio.h>
#include <netinet/in.h>                         // 包含AF_INET相关结构
#include <arpa/inet.h>                      // 包含AF_INET相关操作的函数
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/mman.h>
#include <net/if.h>
#include <sl_types.h>
#include <sl_error.h>
#include <sl_param_vcodec.h>
#include <sl_api_vcodec.h>
#include <sl_config.h>
#include <sl_debug.h>
#include <sl_api_ksysctl.h>
#include <sl_api_filedev.h>
#include <sl_api_memdev.h>
#include <sl_mmutil.h>
#include <sl_msg.h>
#include <pthread.h>
#include <sl_rtsp.h>
#include <sl_param_parser.h>
#include <vpp/sl_param_pv.h>
#include <vpp/sl_api_pv.h>
#include <vpp/sl_param_osd.h>
#include <vpp/sl_api_osd.h>
#include "list_handler.h" 
#include "sl_watchdog.h" 
#include "display.h" 
#include "osd.h" 
#include "gui.h" 
#include "cfginfo.h" 
#include "wifi_sta.h" 
#include <sys/time.h>
#include "audio_ioctl.h" 
#include "app_rtp.h"
#include "ir.h"



//#define APP_CODE
#define WEB_ENABLE
#define KVM_UART
#define APP_IO
#define APP_RTP
//#define  SWIT_MULTICAST
//#define IP_SWITCH
#define ENABLE_GET_IR		//get ir data from rx and to generate ir wave
#define ENABLE_IR_SEND

#define AUDIO_SUPPORT
#define VIDEO_SUPPORT


//For IR
#ifdef ENABLE_GET_IR
static pthread_t get_ir_handler;
#endif
#ifdef ENABLE_IR_SEND
static pthread_t send_ir_handle;
#endif
volatile SL_U32 interface;      //jsson cancel 'static' for bothway ir

#ifdef APP_IO
#include "app_rx_io_ctl.h"
static pthread_t app_rx_io_ctl_handler;
static pthread_t app_rx_light_ctl_handler;
#endif

#ifdef APP_CODE
#include <sl_types.h>

#include "app_rx_signal_ch.h"
#include "app_rx_data_ch.h"

extern unsigned char signal_connect_state;

static pthread_t app_rx_handler;

static pthread_t app_rx_signal_ch_handler;
static pthread_t app_rx_data_ch_handler;
#endif

#include "app_igmp.h"

static pthread_t app_rtp_main_handler;
static pthread_t app_igmp_report_handler;
#ifdef KVM_UART

#include "app_rx_uart.h"

static pthread_t app_rx_uart_handler;

#endif

static pthread_t audio_handle;
static pthread_t  checkWr_handler;
extern void * check_wr_thread(void * Args);

char 	web_flag;

#ifdef WEB_ENABLE
#include "sharemem.h"
#include "init.h"
static pthread_t ConfigHandle;
#endif

client_param_t client_param;


#define NUM_RETRY       50000
#define USLEEP_TIME     5*1000
#define TIMEOUT   -1

#ifdef HANG_PAI
#define MAX_RTSP_TIMEOUT  3
#else
#define MAX_RTSP_TIMEOUT  8
#endif

#define TIME_CHECK_SECONDS 2

static SL_U32 wpa_need_restart = 0;
extern CFG_INFO_S cfginfo;

extern char idr_flag;

char rtspURL_video_eth0[128] = "rtsp://192.168.1.3:8554/ch0"; //video 
char rtspURL_audio_eth0[128] = "rtsp://192.168.1.3:8559/testStream"; //audio 

char rtspURL_video_wlan0[128] = "rtsp://10.10.1.1:8554/ch0"; //video 
char rtspURL_audio_wlan0[128] = "rtsp://10.10.1.1:8559/testStream"; //audio 

volatile static int timeoutCnt = 0;

int fd_config;

static chanVideoPara_s cvp;
static SLVDEC_Cfg_s vdecCfg;
static SLVPP_PV_Cfg_s pv_cfg;
static SLVPP_OSD_Cfg_s osd_cfg;

static SL_U32 im_devman;
static SL_U32 vpu_dev;
static SL_U32 pv_dev;

static SL_U32 im_devman_osd;
static SL_U32 osd_dev;

static SL_U32 g_width;
static SL_U32 g_height;
static SL_S32 g_sysctl_configed;
static SL_U32 g_frameRate;

SL_S32  audio_change = 0;
SL_S32 g_discard_frame; //for eric 

static SL_S32 g_server_connect; //for eric

static SL_U8 g_refreshRate;
static SL_U8 g_interlace;

static SL_S32 g_cnt;
static SL_U32 g_osd_need_reconfig;
static SL_U32 g_osd_overlay;

SL_U32 g_fs;
SL_U32 g_audio_bits;
SL_U32 g_chns;

static SL_U32 netInterface; 

static SL_U32 need_feed_dog = 1;
static LIST_BUFFER_S *list;

static pthread_t iHandle;

static pthread_t rtspHandle_audio;
static pthread_t rtspHandle_video;
static pthread_t watchdogHandle;
static pthread_t wifiHandle;

static pthread_t 	witch_multicast_handler;   //Jason add
static pthread_t 	ip_swtich_handler;
extern void *IP_switch(void);

static SL_POINTER join_ret;
static SL_POINTER join_ret1;

#if 0
const char * server_not_connect ="Not connect to TX";
const char * server_connect ="connect to TX";
const char * server_not_running ="TX Not Running";
const char * server_running ="Check TX's input signal";
#else
const char * server_not_connect ="Searching TX";
const char * server_connect ="Searching TX";
const char * server_not_running ="Searching TX";
const char * server_running ="Check TX's input signal";
const char * hdmi_pullout = "Check TX's input signal";
#endif

char serverip[20] = "192.168.1.200";
char multicast[20] = "239.255.42.44";
//const char * show_text ="你好:hotspot";
const char * show_text ="welcome to silan";

static int process_osd_text_overlay(int x, int y, const char *text);
int process_osd_text_solid(int x, int y, const char *text);
int process_osd_disable(void);
#define DEV_IO_NAME		"/dev/silan_testio"



typedef struct
{
	unsigned int addr;
	unsigned int data;
}SILANWORDR;

/**********************************************************/
#ifdef APP_CODE

//SHOWDATA g_ShowData;

void  *app_rx_main(void)
{
    SL_BOOL bPrevTestMode;
    
    gbTestMode = SL_FALSE;
    bPrevTestMode = SL_FALSE;
    
    g_ShowData.uShowTime = 2; //default 2 seconds

    while(1)
    {
        sleep(1); 
#if 0	
		/*
        if (SL_TRUE == gbTestMode && SL_FALSE==bPrevTestMode)
        {
        	printf("Test Mode On \n");
	        process_osd_text_solid(10, 10, "Test Mode On");
            bPrevTestMode = SL_TRUE;

        }
        else if(SL_FALSE == gbTestMode && SL_TRUE == bPrevTestMode)
        {
	        process_osd_text_solid(10, 10, "Test Mode Off");
	        printf("Test Mode Off \n");
	        sleep(2);
		    process_osd_disable();
        	bPrevTestMode = SL_FALSE;
        }
        */
#endif
		/******************Show Data***************************/
        if(SL_TRUE == g_ShowData.bShowFlag)
        {
			g_ShowData.bShowFlag = SL_FALSE;
			printf("process_osd_text_solid(100, 100, g_ShowData.cText);%s \n", g_ShowData.cText);
            process_osd_text_solid(100, 100, g_ShowData.cText);
			
            if(g_ShowData.uShowTime > 0)
            {
	            sleep(g_ShowData.uShowTime);
		        process_osd_disable();
		        //g_ShowData.bShowFlag = SL_FALSE;
            }
            else
            {
                g_ShowData.uShowTime = 2; //return to default 2 seconds disappear
            }
        }
    }
}

#endif
/***********************************************************/
#if 1
int printf_log(const char *log)
{
	int fd_log = -1;
	int ret = -1;
	int len = -1;
	struct timeval tv1;
	struct timezone tz;
	char buf[40] = {0};
	
	len = strlen(log);
	//len = sizeof(log);
	printf("len = %d \n", len);
	fd_log = open("/tmp/log", O_CREAT|O_RDWR|O_APPEND, \
							S_IRUSR|S_IRGRP|S_IROTH);
	//fd_log = open("/mnt/log", O_CREAT|O_RDWR|O_APPEND, \
							S_IRUSR|S_IRGRP|S_IROTH);
	if (fd_log < 0){
		printf("printf_log open error \n");
		perror("open");
	}
	ret = gettimeofday(&tv1,&tz);
	if (ret < 0){
		printf("printf_log gettimeofday error \n");
		perror("gettimeofday");
	}
	//sprintf(buf, "%ld log: ", tv1.tv_sec-144221050);
	sprintf(buf, "%ld.%ld ", tv1.tv_sec, tv1.tv_usec);
	//sprintf(buf, "%ld log: ", tv1.tv_usec);
	printf("%s", buf);
	ret = write(fd_log, buf, sizeof(buf));
	if (ret < 0)
		printf("write log file error \n");
		
	ret = write(fd_log, log, len);
	if (ret < 0)
		printf("write log file error \n");
		
	ret = write(fd_log, "\n", sizeof("\n"));
	if (ret < 0)
		printf("write log file error \n");
		
	ret = close(fd_log);
	if (ret < 0)
		printf("close log file error \n");
}
#endif

int reboot1(void)
{
	int fd;
	int res = 0;
	SILANWORDR data;

	//sleep(1);
	//system("umount /home");
	fd = open(DEV_IO_NAME, O_RDWR);
	if(fd < 0){
		printf("Open the silan test io error\n");
	}

	data.addr = 3216674868; //0xbfba9034
	data.data = 4294967294; //fffffffe //reset
	res = ioctl(fd, 5, &data);
	if(res < 0){
		printf("### silan test io ioctl error!\n");
		return 1;
	}
	close(fd);

	printf("\n**** reboot now ****\n");

	return 0;
}

/* Init memory control block */
static void initMemCtrl(memCtrl_s *mc, SL_U32 size)
{
	mc->num = 30;
	mc->size = size;
	mc->align = SYS_ALIGN_BYTES;
}

static SL_S32 SLVDEC_setCfg(SLVDEC_Cfg_s *cfg)
{
    memset(cfg,0x00,sizeof(SLVDEC_Cfg_s));
	cfg->chnNum = 1;
    cfg->chnParam[0].rotAngle = 0;
    cfg->chnParam[0].mirDir = 0;
    cfg->chnParam[0].useRot = 0;
    cfg->chnParam[0].useDering = 0;
    cfg->chnParam[0].userPackNeeded = 0;
    cfg->chnParam[0].format = STD_AVC;
    cfg->chnParam[0].picWidth = g_width;
    cfg->chnParam[0].picHeight = (g_height + 15)&(~15); //FIXME

    cfg->chnParam[0].avcExtension = 0; 
    cfg->chnParam[0].mp4DeblkEnable = 0; 
    cfg->chnParam[0].mpeg4Class = 0; 
    cfg->chnParam[0].mapType = 0; 
    return 0;
}

static void  setPvCfg(SLVPP_PV_Cfg_s *cfg)
{
	cfg->sampleMode = VPPSamplingMode_420;
	cfg->colorMode = VPPColorMode_VideoNonInterlace;
	cfg->picWidth = g_width;
	cfg->picHeight = (g_height + 15)&(~15);
	//cfg->picHeight = g_height;
	cfg->blkWindowMode = VPPWindowValueMode_Fixed;
	cfg->blkStartX = 0;
	cfg->blkStartY = 0;
	cfg->blkWidth = g_width;

	cfg->src_picWidth = cfg->picWidth;
	cfg->src_picHeight = cfg->picHeight;

	if(1408==cfg->blkWidth)
		cfg->blkWidth = 1392; //FIXME
#if 0
	cfg->blkHeight = g_height;
	if(1408==cfg->blkWidth)
		cfg->blkWidth = 1392; //FIXME
	cfg->outWindowMode = VPPWindowValueMode_Fixed;
	if(g_width > 1280)
	{
		cfg->outStartX = (1920 - g_width)/2;
		cfg->outStartY = (1080 - g_height)/2;
	}
	else if(g_width > 720)
	{
		if(g_height > 720)
		{
			cfg->outStartX = (1920 - g_width)/2;
			cfg->outStartY = (1080 - g_height)/2;
		}
		else
		{
			cfg->outStartX = (1280 - g_width)/2;
			cfg->outStartY = (720 - g_height)/2;
		}
	}
	else if(g_width > 704)
	{
		cfg->outStartX = 0;
		cfg->outStartY = 0;
	}
	else 
	{
		cfg->outStartX = 0;
		cfg->outStartY = 0;
	}

	cfg->outWidth = g_width;
	if(1408==cfg->outWidth)
		cfg->outWidth = 1392; //FIXME
	cfg->outHeight = g_height;
#else
	if(544 == g_height)
		cfg->blkHeight = 536;
	else if(480 == g_height)
		cfg->blkHeight = 472;
	else if(720 == g_height)
		cfg->blkHeight = 720;
	else
		cfg->blkHeight = 1080;//1072;
		//cfg->blkHeight = g_height ;
		//cfg->blkHeight = 1072;//1080;
	printf("cfg->blkHeight = %d \n", cfg->blkHeight);

	cfg->outStartX = 0;
	cfg->outStartY = 0;
#if 1
	cfg->outWindowMode = VPPWindowValueMode_Relative;
	cfg->outWidth = 4096;
	cfg->outHeight = 4096;
#else
	cfg->outWindowMode = VPPWindowValueMode_Fixed;
	cfg->outWidth = 1920;
	cfg->outHeight = 720;

#endif

#endif

	cfg->refreshRate = g_refreshRate;
	cfg->interlace = g_interlace;

#if 0
	//default
	cfg->con = 50;
	cfg->bri = 50;
	cfg->hue = 0;
	cfg->sat = 50;
#else
	cfg->con = 50;//47; //duibi du 35-65;50
	cfg->bri = 50;//45; //liang du;50
	cfg->hue = 0xff;//0; //se du fixed
	cfg->sat = 70;//70;//35; //baohe du 35-65;35
	cfg->fs = g_fs;
	cfg->chns = g_chns;
	cfg->audio_bits = g_audio_bits; 
	
#endif
}

static SL_S32 sysctl_config(void)
{
	memCtrl_s mctrl;
	SL_S32 ret;
	
	cvp.chan_id = 0;
	cvp.io = IO_OUTPUT;
	cvp.format = FORMAT_YUV_420;
	cvp.reso = RESO_USER_DEFINED;
	cvp.reso_wi = g_width;
	cvp.reso_hi = (g_height + 15)&(~15);//g_height;    
	//cvp.reso_hi = g_height;
	ret = SLSYSCTL_open(&im_devman, ID_MEMORY, &cvp, sizeof(cvp));
	if (SL_NO_ERROR != ret)
		return -1;

	ret = SLSYSCTL_open(&vpu_dev, ID_VPU0DEC, &cvp, sizeof(cvp));
	if (SL_NO_ERROR != ret)
		goto close_im;

	ret = SLSYSCTL_open(&pv_dev, ID_VPP_PV, &cvp, sizeof(cvp));
	if (SL_NO_ERROR != ret)
		goto close_vdec;    

	ret = SLSYSCTL_bind(im_devman, 0, vpu_dev, 0);
	if (SL_NO_ERROR != ret)
		goto close_pv_dev;

	ret = SLSYSCTL_bind(vpu_dev, 0, pv_dev, 0); 
	if (SL_NO_ERROR != ret)
		goto unbind_vdec;

	initMemCtrl(&mctrl, 16*1024*1024);
	ret = SLMDEV_setBuffer(im_devman, &mctrl);
	if (SL_NO_ERROR != ret)
		goto unbind_pv_dev;

	SLVDEC_setCfg(&vdecCfg);
	ret = SLVPUDEC_config(vpu_dev, &vdecCfg);
	if (SL_NO_ERROR != ret)
		goto unbind_pv_dev;

	setPvCfg(&pv_cfg);

	ret = SLVPP_PV_config(pv_dev, &pv_cfg);
	if (SL_NO_ERROR != ret)
		goto unbind_pv_dev;

	printf("******sysctl_config*****\n");
	
	return 0;

unbind_pv_dev:
	SLSYSCTL_unbindFromAll(pv_dev);

unbind_vdec:
	SLSYSCTL_unbindFromAll(vpu_dev);

close_pv_dev:
	SLSYSCTL_close(pv_dev);

close_vdec:
	SLSYSCTL_close(vpu_dev);

close_im:
	SLSYSCTL_close(im_devman);

	return -1;
}

static SL_S32 trigger(SL_S32 first_time)
{
	SL_S32 ret;
	
	printf("first_time : %d \n", first_time);
	if(first_time) {
		if(sysctl_config())
		{
			log_err("sysctl_config failed");
			reboot1();
		}

	} else {
		usleep(20000); //TODO
		ret = SLSYSCTL_stopProcess(im_devman); 
		if (SL_NO_ERROR != ret) {
			log_err("Input FDEV stop process failed");
			reboot1();
		}
		usleep(200000); //TODO
		SLSYSCTL_unbindFromAll(pv_dev);
		SLSYSCTL_unbindFromAll(vpu_dev);
		SLSYSCTL_close(pv_dev);
		SLSYSCTL_close(vpu_dev);
		SLSYSCTL_close(im_devman);
		if(sysctl_config())
		{
			log_err("sysctl_config failed");
			reboot1();
		}
	}

	printf("\n **trigger \n");
	
	return 0;
}

#if 1

//pthread
//static SL_POINTER wacPushFrameToMDev(SL_POINTER Args)
void wacPushFrameToMDev(unsigned char *pframe, unsigned int uiSize)
{
	SL_S32 ret;
	void *buf;
	SL_U32 cpSize; 
	SL_U32 size; 
	unsigned char *data;
	//unsigned char *pframe;
	H264_APPEND_INFO_s *append_info;
	SL_S32 idr_frame;
	SL_S32 first_time;
	static SL_S32 discard_p_frame = 0;
	static SL_S32 tmp_fs = 0;
	static SL_S32  idr_got = 0;
	static SL_S32 frameCount = 0;
	static struct timeval time_begin = {0};
	static struct timeval time_end = {0};
	
	SL_S32 interval;
	SL_U32 total_frame;
	float interval_second;
	static int i=1;
	size = uiSize;

    //printf("wac Get a Frame size = %d \n",size);
    //return 0;
    
	//while(1)
	{
		//for eric 
		timeoutCnt = 0;
		//printf("wac working, g_framerate= %d\n",g_frameRate);
		//sleep(1);

		if(g_frameRate > 0)
		{
			total_frame = TIME_CHECK_SECONDS * g_frameRate;
			if(!frameCount)
				gettimeofday(&time_begin, 0);
			frameCount ++;
			if(frameCount >= total_frame)
			{
				//printf("frameCount:%d\n",frameCount);
				gettimeofday(&time_end, 0);
				interval = (time_end.tv_sec-time_begin.tv_sec)*1000+(time_end.tv_usec-time_begin.tv_usec)/1000;

				//printf("time_end.tv_sec:%d\n",time_end.tv_sec);
				//printf("time_begin.tv_sec:%d\n",time_begin.tv_sec);

				//printf("interval_ms:%d\n",interval);
				interval_second = ((float)interval)/1000;
				//printf("interval_second:%f\n",interval_second);
				g_discard_frame = g_frameRate - (SL_S32)(total_frame/interval_second); 
				if(g_discard_frame > 5)
				{
					printf("===g_discard_frame:%d\n",g_discard_frame);
					if(i==1)
					{
						i=0;
						HDMI_HotPlug();
					}
				}
				frameCount = 0;
			}
		}

        /*Removed debug info for speed up
        int i;
        for(i=0;i<50;i++)
            printf(" %02x ", pframe[i]);
        */

		data = (unsigned char *)pframe + 4;
		//printf("data : %d \n", *data);
		if (0x67 == *data)
		{

		    /*Removed debug info for speed up
		    printf(" \n idr frame size = %d \n",size);
		    
		    for(i=(size - sizeof(H264_APPEND_INFO_s));i<size;i++)
                printf(" %02x ", pframe[i]);
		    */
			append_info = (H264_APPEND_INFO_s *)(pframe + size - sizeof(H264_APPEND_INFO_s));
			/*Removed debug info for speed up
			unsigned char *temp = (unsigned char *)append_info;
					
			for(i=0;i<sizeof(H264_APPEND_INFO_s);i++)
	            printf("%02x ",temp[i]);
            */
            
			//parse audio
			g_fs = append_info->fs;
			g_audio_bits = append_info->audio_bits;
			g_chns = append_info->chns;

			//FIXME
			if (g_chns > 8)
			{
				printf("abnomal g_chns:%d\n",g_chns);
				goto step;
			}
			if (g_audio_bits > 24)
			{
				printf("abnomal g_audio_bits:%d\n",g_audio_bits);
				goto step;
			}
			switch(g_fs)
			{
				case 44100:
				case 48000:
				case 192000:
					break;

				default:
					printf("abnomal g_fs:%d\n",g_fs);
					goto step;
			}
			
#if 1
			//printf("g_fs : %d \n", g_fs);
			//printf("g_audio_bits : %d \n", g_audio_bits);
			//printf("g_chns : %d \n", g_chns);
			//if ((tmp_fs != g_fs) && ((44100 == g_fs) || (48000 == g_fs) || (192000 == g_fs)))
			if (tmp_fs != g_fs)
			{
				audio_change = 1;
				//rtsp_audio_client_config(g_fs, g_audio_bits, g_chns);
				printf("g_fs : %d \n", g_fs);
				printf("g_audio_bits : %d \n", g_audio_bits);
				printf("g_chns : %d \n", g_chns);
				audio_config(g_fs, g_audio_bits, g_chns);
				//audio_config(44100, 16, 2);
				printf("////////////////////////////g_fs : %d \n", g_fs);
				//sysctl_config();
				tmp_fs = g_fs;
			}
			
#endif
		
step:
			//printf("append_info->width : %d \n", append_info->width);
			//printf("append_info->height : %d \n", append_info->height);
			//printf("append_info->frameRate : %d \n", append_info->frameRate);
			
			if((append_info->width > 1920) || (append_info->height > 1088) || (append_info->frameRate > 60) || (0 == append_info->width) || (0 == append_info->height)) 
			{
				log_note("abnoamrl width or height or frameRate.maybe reason of miss data\n");
				discard_p_frame = 1;
				return 0;
			}

			//else if((g_width != append_info->width ) || (g_height != append_info->height) || (g_frameRate != append_info->frameRate))
			else if ((g_width != append_info->width ) || (g_height != append_info->height) || (g_frameRate != append_info->frameRate) || audio_change)
			{
				audio_change = 0;
				g_discard_frame = 0;
				g_sysctl_configed = 0;
				list_flush_data(list);
				printf("---g_width : %d -----\n", g_width);
				if(!g_width)
					first_time = 1;
				else
					first_time = 0;
				g_width = append_info->width;
				g_height = append_info->height;
				g_frameRate = append_info->frameRate;
				g_refreshRate = append_info->refreshRate;
				g_interlace = append_info->interlace;

				printf("g_width:%d\n",g_width);
				printf("g_height:%d\n",g_height);
				printf("g_frameRate:%d\n",g_frameRate);
				printf("g_refreshRate:%d\n",g_refreshRate);
				printf("g_interlace:%d\n",g_interlace);

				trigger(first_time);
				g_sysctl_configed = 1;
				g_osd_need_reconfig = 1;
			}
			idr_frame = 1;
			idr_got = 1;
			discard_p_frame = 0;
			//printf("*****************idr*****************");
			idr_flag = 0;

		}
		else
		{
	    	//printf(" \n ************not idr frame : %d \n", idr_flag);
	    	if (idr_flag) //wang
				return 0;
			
			if(!g_sysctl_configed)
			{
			    printf("not configed \n");
				return 0;
            }
			//printf("frame : %d \n", discard_p_frame);
			if(discard_p_frame)
			{
			    printf("discard frame \n");
				return 0;
			}

			idr_frame = 0;
		}
		if(!idr_frame)
			cpSize = size;
		else
		{
			cpSize = size - sizeof(H264_APPEND_INFO_s);
			//printf("idr Size:%d\n",cpSize);
		}

		if(!idr_got)
			return 0;

       //printf("allocate block \n");
        
tryAgain:	
		ret = SLMDEV_mallocBlockWrite(im_devman, &buf, cpSize);
		if (SL_NO_ERROR != ret) {		
			printf("pv SLMDEV_mallocBlockWrite failed \n");
			usleep(10000);
			reboot1();
			//goto tryAgain;
		}

		//printf("allocate block ok\n");
		memcpy(buf, pframe, cpSize);

		ret = list_push_data(list, buf);
		if(ret)
		{
			printf("fail to push data\n");
			return NULL;
		}
		//else
		//{
		//    printf("list push data ok \n");
		//}
	}
	
	return 0;
}

#else
static int wacPushFrameToMDev(unsigned char *pframe, int  size)
{
	SL_S32 ret;
	void *buf;
	SL_U32 cpSize; 
	unsigned char *data;
	H264_APPEND_INFO_s *append_info;
	SL_S32 idr_frame;
	SL_S32 first_time;
	static SL_S32 discard_p_frame = 0;

	static SL_S32  idr_got = 0;
	static SL_S32 frameCount = 0;
	static struct timeval time_begin = {0};
	static struct timeval time_end = {0};
	SL_S32 interval;
	SL_U32 total_frame;
	float interval_second;
	static SL_S32  audio_change = 0;
#if 0
	static int cnt = 0;
	cnt++;
	printf("wac:%d\n",cnt);
#endif
	timeoutCnt = 0;
	//printf("timeoutCnt = 0\n");
	if(NULL==pframe || size<1)
	{
		printf("Err: f:%s() invalide param, pframe:%p size:%d.\n", __func__, pframe, size);
		return -1;
	}
#ifdef APP_CODE
	gbBandwidthDetectMode=SL_FALSE;
	HDMI_light_on();
#endif
	if(g_frameRate > 0)
	{
		total_frame = TIME_CHECK_SECONDS * g_frameRate;
		if(!frameCount)
			gettimeofday(&time_begin, 0);
		frameCount ++;
		if(frameCount >= total_frame)
		{
			//printf("frameCount:%d\n",frameCount);
			gettimeofday(&time_end, 0);
			interval = (time_end.tv_sec-time_begin.tv_sec)*1000+(time_end.tv_usec-time_begin.tv_usec)/1000;

			//printf("time_end.tv_sec:%d\n",time_end.tv_sec);
			//printf("time_begin.tv_sec:%d\n",time_begin.tv_sec);

			//printf("interval_ms:%d\n",interval);
			interval_second = ((float)interval)/1000;
			//printf("interval_second:%f\n",interval_second);
			g_discard_frame = g_frameRate - (SL_S32)(total_frame/interval_second); 
			//printf("g_discard_frame:%d\n",g_discard_frame);
			if(g_discard_frame > 5)
				printf("g_discard_frame:%d\n",g_discard_frame);
			frameCount = 0;

		}
	}
	//printf("-------------------------------------\n");
	data = (unsigned char *)pframe + 4;
	if( 0x67 == *data)
	{
		
#if 0
		append_info = (H264_APPEND_INFO_s *)(pframe + size - sizeof(H264_APPEND_INFO_s));
		if ((192000 == g_fs || 44100 == g_fs || 48000 == g_fs) && \
			(1 == g_chns || 2 == g_chns || 4 == g_chns) && \
			(16 == g_audio_bits)) //for wang 180601
		{
			if((g_fs != append_info->fs) || (g_chns != append_info->chns) || (g_audio_bits != append_info->audio_bits))
				audio_change = 1;
		}
#endif	
		//parse audio
		g_fs = append_info->fs;
		g_audio_bits = append_info->audio_bits;
		g_chns = append_info->chns;
	
		//FIXME
		if(g_chns > 8)
		{
			printf("abnomal g_chns:%d\n",g_chns);
			goto step;
		}
		if(g_audio_bits > 24)
		{
			printf("abnomal g_audio_bits:%d\n",g_audio_bits);
			goto step;
		}
		switch(g_fs)
		{
			case 44100:
			case 48000:
			case 192000:
				break;

			default:
				printf("abnomal g_fs:%d\n",g_fs);
				goto step;
		}
		
		printf("g_fs = %d \n", g_fs);
		printf("g_audio_bits = %d \n", g_audio_bits);
		printf("g_chns = %d \n", g_chns);
		
		//audio_config(g_fs, g_audio_bits, g_chns);
		//rtsp_audio_client_config(g_fs, g_audio_bits, g_chns);
step:
		if((append_info->width > 1920) || (append_info->height > 1088) || (append_info->frameRate > 60)) {
			log_note("abnoamrl width or height or frameRate.maybe reason of miss data\n");
			discard_p_frame = 1;
			return 0;
		}

		else if((g_width != append_info->width ) || (g_height != append_info->height) || (g_frameRate != append_info->frameRate) || audio_change)
		{
			audio_change = 0;
			g_discard_frame = 0;
			g_sysctl_configed = 0;
			list_flush_data(list);
			if(!g_width)
				first_time = 1;
			else
				first_time = 0;
			g_width = append_info->width;
			g_height = append_info->height;
			g_frameRate = append_info->frameRate;
			g_refreshRate = append_info->refreshRate;
			g_interlace = append_info->interlace;

			printf("g_width:%d\n",g_width);
			printf("g_height:%d\n",g_height);
			printf("g_frameRate:%d\n",g_frameRate);
			printf("g_refreshRate:%d\n",g_refreshRate);
			printf("g_interlace:%d\n",g_interlace);

			trigger(first_time);
			g_sysctl_configed = 1;
			g_osd_need_reconfig = 1;
		}
		idr_frame = 1;
		idr_got = 1;
		discard_p_frame = 0;
	}
	else
	{
		if(!g_sysctl_configed)
			return 0;


		if(discard_p_frame)
			return 0;

		idr_frame = 0;
	}

	if(!idr_frame)
		cpSize = size;
	else
	{
		cpSize = size - sizeof(H264_APPEND_INFO_s);
		//printf("idr Size:%d\n",cpSize);
	}

	if(!idr_got)
		return 0;
#if 0
	if(g_discard_frame > 5)
		return 0;
#endif
tryAgain:
	ret = SLMDEV_mallocBlockWrite(im_devman, &buf, cpSize);
	if (SL_NO_ERROR != ret) {		
		printf("pv SLMDEV_mallocBlockWrite failed \n");
		//usleep(10000);
		usleep(2000);
		goto tryAgain;
	}

	memcpy(buf, pframe, cpSize);
#if 0
	{
		int k;
		unsigned char * buf1;
		buf1 = (unsigned char *)buf;
		printf("\n");
		for(k=0;k<10;k++)
			printf("%X-",buf1[k]);
		printf("\n");

	}
#endif
	ret = list_push_data(list, buf);
	if(ret)
	{
		printf("fail to push data\n");
		return -1;
	}

	return 0;
}

#endif

//#ifndef HANG_PAI
#if 1
static SL_POINTER MDEV_Input_ThreadFunc(SL_POINTER Args)
{	
	void *buf;
	//SL_U32 interval;
	SL_S32 ret;
	static SL_S32 cnt = 0;
	//static struct timeval time_begin = {0};
	//static struct timeval time_end = {0};

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );
	while (1) {

		if(!g_sysctl_configed)
		{
			usleep(2000);
			cnt = 0;
			continue;
		}

		ret = list_fetch_data(list, &buf);
		if(ret)
		{
			//printf(" failed to fetch data\n");
			//usleep(8000);
			usleep(5000);
			continue;
		}

		//FIXME
		if(!g_cnt)
		{
			g_server_connect = 1;
			if(!g_osd_overlay) {
				if(osd_dev) 
				{
					//printf("\n**** stop osd\n");
					SLSYSCTL_stopProcess(osd_dev);

				}
			}
			g_cnt = 1;
		}

		SLMDEV_freeBlockWrite(im_devman, buf);
		//printf_log("mdev write");
			//printf("mdev write\n");
#if 0
		if(!cnt)
		{
			//		gettimeofday(&time_begin, 0);
			cnt = 1;
		} else {
			//		gettimeofday(&time_end, 0);
			//		interval = (time_end.tv_sec-time_begin.tv_sec)*1000+(time_end.tv_usec-time_begin.tv_usec)/1000;
#if 0
			if(interval > 50)
			{
				printf(" interval: %d\n",interval );
				//cnt = 0;
			}
#endif
#if 0
			//FIXME
			if(30 == g_frameRate)
			{
				if(interval < 33)
					usleep(33000-interval*1000); //30f/s
			} else if(25 == g_frameRate) {
				if(interval < 40)
					usleep(40000-interval*1000);//25f/s
			}
#endif
			//		gettimeofday(&time_begin, 0);

		}
#endif

	}

	return SL_NO_ERROR;
}
#else
static SL_POINTER MDEV_Input_ThreadFunc(SL_POINTER Args)
{	
	void *buf;
	SL_U32 interval;
	SL_U32 interval_standard = 0;
	SL_S32 ret;
	static struct timeval time_begin = {0};
	static struct timeval time_end = {0};
	while (1) {

		if(!g_sysctl_configed)
		{
			usleep(2000);
			g_cnt = 0;
			continue;
		}
		if(!g_cnt)
		{
			ret = list_fetch_data(list, &buf);
			if(ret)
			{
				usleep(2000);
				continue;
			}
			//FIXME
			if(!g_osd_overlay) {
				if(osd_dev) 
				{
					//printf("stop osd\n");
					SLSYSCTL_stopProcess(osd_dev);

				}
			}

			SLMDEV_freeBlockWrite(im_devman, buf);
			if(30 == g_frameRate) {
				interval_standard = 30;
			} else if(25 == g_frameRate) {
				interval_standard = 36;
			} else
				interval_standard = 30; //FIXME
		}

		if(!g_cnt)
		{
			gettimeofday(&time_begin, 0);
			g_cnt = 1;
		} else {
			gettimeofday(&time_end, 0);
			interval = (time_end.tv_sec-time_begin.tv_sec)*1000+(time_end.tv_usec-time_begin.tv_usec)/1000;
			if(interval >= interval_standard)
			{
				ret = list_fetch_data(list, &buf);
				if(ret)
				{
					//printf("list_fetch failed\n");
					usleep(2000);
					continue;
				}

				SLMDEV_freeBlockWrite(im_devman, buf);

				memcpy(&time_begin, &time_end, sizeof(struct timeval));
				usleep(500);
				//usleep(10000);
			}
			else
			{
				//usleep(1000);
				usleep(500);
			}
		}
		//usleep(2000);
	}

	return SL_NO_ERROR;
}
#endif

static void initMemCtrl_osd(memCtrl_s *mc, SL_U32 size)
{
	mc->num = 2;
	mc->size = size;
	mc->align = SYS_ALIGN_BYTES;
}

static void  setOsdCfg(SLVPP_OSD_Cfg_s *cfg)
{
	cfg->colorFormat = VPPColorFormat_32BPP_8888;
	cfg->colorMode = VPPColorMode_TrueColor;
	cfg->picWidth = OSD_WIDTH;
	cfg->picHeight = OSD_HEIGHT;
	cfg->blkWindowMode = VPPWindowValueMode_Fixed;
	cfg->blkStartX = 0;
	cfg->blkStartY = 0;
	cfg->blkWidth = OSD_WIDTH;
	cfg->blkHeight = OSD_HEIGHT;
	cfg->outWindowMode = VPPWindowValueMode_Relative;
	cfg->outStartX = 0;
	cfg->outStartY = 0;
	cfg->outWidth = 4096;
	cfg->outHeight = 4096;
}

static SL_S32 osd_sysctl_config(void)
{
	memCtrl_s mctrl;
	chanVideoPara_s cvp;
	SL_S32 ret;

	cvp.chan_id = 0;
	cvp.io = IO_OUTPUT;
	cvp.format = FORMAT_RGB_8888;
	cvp.reso = RESO_USER_DEFINED;
	cvp.reso_wi = OSD_WIDTH;
	cvp.reso_hi = OSD_HEIGHT;    

	ret = SLSYSCTL_open(&im_devman_osd, ID_MEMORY, &cvp, sizeof(cvp));
	if (SL_NO_ERROR != ret)
		return -1;

	ret = SLSYSCTL_open(&osd_dev, ID_VPP_OSD, &cvp, sizeof(cvp));
	if (SL_NO_ERROR != ret)
		goto close_im;    

	ret = SLSYSCTL_bind(im_devman_osd, 0, osd_dev, 0);
	if (SL_NO_ERROR != ret)
		goto close_osd_dev;

	initMemCtrl_osd(&mctrl, OSD_WIDTH*OSD_HEIGHT*4);
	ret = SLMDEV_setBuffer(im_devman_osd, &mctrl);
	if (SL_NO_ERROR != ret)
		goto close_osd_dev;

	setOsdCfg(&osd_cfg);
	ret = SLVPP_OSD_config(osd_dev, &osd_cfg);
	if (SL_NO_ERROR != ret)
		goto unbind_osd_dev;

	return 0;
	
unbind_osd_dev:
	SLSYSCTL_unbindFromAll(osd_dev);

close_osd_dev:
	SLSYSCTL_close(osd_dev);

close_im:
	SLSYSCTL_close(im_devman);

	return -1;
}

static int process_osd_text_overlay(int x, int y, const char *text)
{
	SL_S32 ret;
	void *buf;
	SL_S32 i;
	char *dst;
	int color_fg, color_bk;

	if(g_osd_need_reconfig)
	{
		setOsdCfg(&osd_cfg);
		ret = SLVPP_OSD_config(osd_dev, &osd_cfg);
		if (SL_NO_ERROR != ret)
		{
			log_err("SLVPP_OSD_config");
			reboot1();
			return -1;
		}
		g_osd_need_reconfig = 0;
	}

tryAgain:
	ret = SLMDEV_mallocBlockWrite(im_devman_osd, &buf, OSD_WIDTH*OSD_HEIGHT*4);
	if (SL_NO_ERROR != ret) {		
		log_note("osd SLMDEV_mallocBlockWrite failed");
		usleep(100000); //FIXME
		goto tryAgain;
	}
	//printf("osd mdev malloc\n");

	// let vpp use the ddr 
	dst = buf;
	for(i=0; i<OSD_HEIGHT/2; i++)
	{
		memset(dst, 0x00, OSD_WIDTH*8);
		dst += OSD_WIDTH*8;
		usleep(1000);
	}

	set_osd_buf(buf);

	color_fg = COLOR_WHITE_SOLID;
	color_bk = COLOR_WHITE_TRANSPARENT;

	text_show(x, y, text, color_fg, color_bk, strlen(text)*16);

	SLMDEV_freeBlockWrite(im_devman_osd, buf);

	g_osd_overlay = 1;

	//printf("osd mdev free\n");

	return 0;

}

int process_osd_text_solid(int x, int y, const char *text)
{
	SL_S32 ret;
	void *buf;
	SL_S32 i;
	char *dst;
	int color_fg, color_bk;

	if(g_osd_need_reconfig)
	{
		printf("g_osd_need_reconfig\n");
		setOsdCfg(&osd_cfg);
		ret = SLVPP_OSD_config(osd_dev, &osd_cfg);
		if (SL_NO_ERROR != ret)
		{
			log_err("SLVPP_OSD_config");
			reboot1();
			return -1;
		}
		g_osd_need_reconfig = 0;
	}

tryAgain:	
	ret = SLMDEV_mallocBlockWrite(im_devman_osd, &buf, OSD_WIDTH*OSD_HEIGHT*4);
	if (SL_NO_ERROR != ret) {		
		log_note("osd SLMDEV_mallocBlockWrite failed");
		usleep(100000); //FIXME
		goto tryAgain;
	}
	//printf("osd mdev malloc\n");

	// let vpp use the ddr 
	dst = buf;
	for(i=0; i<OSD_HEIGHT/2; i++)
	{
		//memset(dst, 0xff, OSD_WIDTH*8); //white
		memset(dst, 0x00, OSD_WIDTH*8); //black
		dst += OSD_WIDTH*8;
		usleep(1000);
	}

	set_osd_buf(buf);

	color_fg = COLOR_WHITE_SOLID;
	color_bk = COLOR_BLACK_SOLID;


	text_show(x, y, text, color_fg, color_bk, strlen(text)*16);
	SLMDEV_freeBlockWrite(im_devman_osd, buf);
	g_osd_overlay = 0;

	if(pv_dev) 
	{
		printf("stop pv\n");
		SLSYSCTL_stopProcess(pv_dev);

	}
	//printf("osd mdev free\n");

	return 0;

}
#if 0
static int process_osd_image(int icon_id)
{
	SL_U8 *imageData;
	SL_S32 ret;
	void *buf;
	SL_S32 i;
	char *dst, *src;

	if(g_osd_need_reconfig)
	{
		setOsdCfg(&osd_cfg);
		ret = SLVPP_OSD_config(osd_dev, &osd_cfg);
		if (SL_NO_ERROR != ret)
		{
			log_err("SLVPP_OSD_config");
			return -1;
		}
		g_osd_need_reconfig = 0;
	}

	imageData = osd_display_icon_data(icon_id);
	if(!imageData)
		return -1;
tryAgain:	
	ret = SLMDEV_mallocBlockWrite(im_devman_osd, &buf, 1280*720*4);
	if (SL_NO_ERROR != ret) {		
		log_note("osd SLMDEV_mallocBlockWrite failed");
		usleep(100000); //FIXME
		goto tryAgain;
	}
	printf("osd mdev malloc\n");
	memcpy(buf, imageData, OSD_WIDTH*OSD_HEIGHT*4);
	
	// let vpp use the ddr 
	dst = buf;
	src = imageData;
	for(i=0; i<OSD_HEIGHT*4; i++)
	{
		memcpy(dst, src, OSD_WIDTH);
		dst += OSD_WIDTH;
		src += OSD_WIDTH;
		usleep(1000);
	}
	
	if(pv_dev)
		SLSYSCTL_stopProcess(pv_dev);

	SLMDEV_freeBlockWrite(im_devman_osd, buf);
	printf("osd mdev free\n");

	return 0;

}
#endif

int process_osd_disable(void)
{
	if(osd_dev)
		SLSYSCTL_stopProcess(osd_dev);

	return 0;
}

static SL_POINTER rtspOpen_video(SL_POINTER Args)
{
	printf("rtspOpen_video\n");
	if(INTERFACE_WLAN0 == netInterface)
	{
		rtsp_open("video", rtspURL_video_wlan0, wacPushFrameToMDev, &client_param);
		printf("rtspURL_video:%s\n",rtspURL_video_wlan0);
	}
	else
	{
		rtsp_open("video", rtspURL_video_eth0, wacPushFrameToMDev, &client_param);
		printf("rtspURL_video:%s\n",rtspURL_video_eth0);
	}
	return NULL;
}

static SL_POINTER rtspOpen_audio(SL_POINTER Args)
{
	printf("rtspOpen_audio\n");
	if(INTERFACE_WLAN0 == netInterface)
		rtsp_open_audio("audio", rtspURL_audio_wlan0, NULL, &client_param);
	else
		rtsp_open_audio("audio", rtspURL_audio_eth0, NULL, &client_param);

	return NULL;
}

void signalioHandle(int signo) 
{
	reboot1();
}

void signalHandle(int signo)
{
	reboot1();
}

static SL_POINTER  watchdog_handle(SL_POINTER Args)
{
	int fd;
	int value, count;
	char buf[] = "hello"; 

	fd = open("/dev/silan-watchdog", O_RDWR);
	if ( -1 == fd)
	{
		printf("open silan-watchdog failed\n");
		return NULL;
	}

	value = 30;
	ioctl(fd, WDIOC_SETTIMEOUT, &value);
	ioctl(fd, WDIOC_GETTIMEOUT, &value);
	//printf("timeout:%d\n", value);

	while(1)
	{
		sleep(1);
		ioctl(fd, WDIOC_GETTIMELEFT, &value);
		//printf("timeleft:%d\n", value);

		if(need_feed_dog)
		{
			if(value <= 25)
			{
				count = write(fd, buf, sizeof(buf)); //feed dog
				//printf("success to feed dog\n");
			}
		}
	}

	return NULL;
}

static int  wifi_sta_start(void)
{
	char ip[20];
	int ret;

	//printf("%s\n",__func__);
	ret = wifi_sta_start_wpa();
	if(ret)
		printf("failed to start_wpa\n");

	ret = wifi_sta_start_udhcpc();
	if(ret)
		printf("failed to start_udhcpc\n");

	sleep(12);

	if(!wifi_sta_get_ip_addr(ip) && (strcmp(ip, "127.0.0.1") != 0)){
		printf("ip:%s\n",ip);
	} else {
		printf("failed to get ip\n");
	}

	return 0;
}

static SL_POINTER  wifi_handle(SL_POINTER Args)
{
	static int timeout = 5;
	char ip[20];
	int ret;

	while(1)
	{
		if(wpa_need_restart)
		{
			wpa_need_restart = 0;

			wifi_sta_stop_wpa();
			wifi_sta_stop_udhcpc();
			sleep(2);
			ret = wifi_sta_start_wpa();
			if(ret)
				printf("failed to start_wpa\n");
			ret = wifi_sta_start_udhcpc();
			if(ret)
				printf("failed to start_udhcpc\n");
			timeout -- ;
			//sleep(8);
		}
#if 0
		if(!wifi_sta_get_ip_addr(ip) && (strcmp(ip, "127.0.0.1") != 0)){
			//printf("ip:%s\n",ip);
			//timeout = 5;
			timeout = 5;
		} else {
#if 0
			if(0 == timeout)
				reboot1();
#endif
			wifi_sta_stop_wpa();
			wifi_sta_stop_udhcpc();
			sleep(2);
			ret = wifi_sta_start_wpa();
			if(ret)
				printf("failed to start_wpa\n");
			ret = wifi_sta_start_udhcpc();
			if(ret)
				printf("failed to start_udhcpc\n");
			timeout -- ;
			sleep(8);
		}
#endif
		usleep(500000);
	}

	return NULL;
}

/*******jason add 20180830*****/
static int rx_witch_multicast_main(void)
{
	printf("rx_witch_multicast_main  start \n");

	rx_witch_multicast();

	return 0;
}

/* For usysctl testing */
int main(int argc, char* argv[])
{
	SL_S32 ret = -1, i;
	
	RTSP_STATE_e state;

	char configs[128];
	printf("\n********************system starting***************************\n");
	printf(PRINT_VERSION);
	printf("\n---------------------------------------------------------------\n");
	//AppWriteCfgInfotoFile();
#if 0
	ret = InitCfgInfo(&fd_config);
	if(!ret) {

		sprintf(rtspURL_video,  "%s%s%s", "rtsp://",cfginfo.serverip,":8554/ch0");

		sprintf(rtspURL_audio,  "%s%s%s", "rtsp://",cfginfo.serverip,":8559/testStream");

		//sprintf(configs, "%s%s", "ifconfig eth0 ",cfginfo.ip);
		//system(configs);

		sprintf(cfginfo.ip,  "%s", "192.168.1.10");

		update_cfg_info(fd_config, &cfginfo);

	}
	close(fd_config);
#endif
	ret = SLSYSCTL_openSysCtlLib();
	if (SL_NO_ERROR != ret)
		return 0;
	//printf("rtspURL_video:%s\n",rtspURL_video);
	//printf("rtspURL_audio:%s\n",rtspURL_audio);

	list = list_init();
	if(!list)
	{
		log_err("fail to create list\n");
		return 0;
	}
	
#ifdef 	WEB_ENABLE
	InitShareMem();
	AppInitCfgInfoDefault();
    printf("cfg init ok \n");
    ret = AppInitCfgInfoFromFile(&fd_config);
	AppWriteCfgInfotoFile();
    printf("ret = %d\n",ret);
    if(ret<0)
    {
        if(NULL!=fd_config)
            close(fd_config);
        printf("build default config.conf \n");
        AppWriteCfgInfotoFile();
    }
    else
    {
        printf("cfg get from file \n");
        close(fd_config);
    }
    strcpy(multicast, share_mem->sm_eth_setting.strEthMulticast);
    //if(strcmp("192.168.1.5",share_mem->sm_eth_setting.strEthIp)!=0)
	init_eth();//   zhou
	
#endif
	osd_display_init();
	osd_sysctl_config();
	process_osd_text_solid(10, 10, OSD_VERSION);
	sleep(3);
#ifdef WEB_ENABLE
	process_osd_text_solid(10, 10, share_mem->sm_eth_setting.strEthIp);
	//sleep(1);
#endif
#if 1
	ret = pthread_create(&watchdogHandle, NULL, watchdog_handle, NULL);
	if (ret) {
		log_err("Failed to Create watchdogHandle Thread\n");
		reboot1();
		return ret;
	}
#endif

	
#ifdef KVM_UART
	ret = pthread_create(&app_rx_uart_handler, NULL, app_rx_uart_main, NULL);
	if (ret) {
		log_err("Failed to Create app_rx_uart_main Thread\n");
		return ret;
	}
#endif

/******** jason add 20180830 *****************/
#ifdef SWIT_MULTICAST
    printf("rx_witch_IpAddress_main test start : [main]\n");   
	ret = pthread_create(&witch_multicast_handler, NULL, rx_witch_multicast_main, NULL);
	if (ret) {
		log_err("Failed to Create tx_witch_IpAddress_main Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#ifdef IP_SWITCH
    printf("IP_SWITCH test start : [main]\n");   
	ret = pthread_create(&ip_swtich_handler, NULL, IP_switch, NULL);
	if (ret) {
		log_err("Failed to Create tx_witch_IpAddress_main Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif


/********* Jason add for  bothway IR test 20181121 ***********************/
#ifdef ENABLE_GET_IR
	ret = pthread_create(&get_ir_handler, NULL, get_ir, NULL);
	if (ret) {
		log_err("Failed to Create get_ir Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif
#ifdef ENABLE_IR_SEND
	ret = pthread_create(&send_ir_handle, NULL, send_ir, NULL);
	if (ret) {
		log_err("Failed to send_ir Thread\n");
		reboot1();
		return ret;
	}
#endif
/**************** End for bothway ir ************************************/	
//	while(1) sleep(1);
	
#ifdef APP_IO
#if 0
	ret = pthread_create(&app_rx_light_ctl_handler, NULL, app_rx_light_ctl_main, NULL);
	if (ret) {
		log_err("Failed to Create app_rx_light_ctl_main Thread\n");
		return ret;
	}
#endif

#if 0
	ret = pthread_create(&app_rx_io_ctl_handler, NULL, app_rx_io_ctl_main, NULL);
	if (ret) {
		log_err("Failed to Create rtspOpen Thread\n");
		return ret;
	}
#endif
	sleep(1); //wait IO init ok
	//signal_light_flash();
#endif
	
#ifdef WEB_ENABLE
	ret = pthread_create(&ConfigHandle, NULL, sharemem_handle, NULL);
	if (ret) {
		log_err("Failed to Create Config Handle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#if 0
	memset(&client_param, 0x00, sizeof(client_param_t));
#if 1
	netInterface = INTERFACE_ETH0; 
#else
	netInterface = INTERFACE_WLAN0; 
#endif
	client_param.interface = netInterface;
	client_param.overTCP = 1;//0;//1; //0:UDP 1:TCP

	if(netInterface == INTERFACE_WLAN0) //wifi
	{
		sleep(4);	//wait 8192 detected completely
		wifi_sta_start();
	}
	
	if (INTERFACE_ETH0 == netInterface) //net
	{
		
#ifdef WEB_ENABLE
		strcpy(serverip, share_mem->sm_eth_setting.strEthGateway);
		sprintf(rtspURL_video_eth0,"rtsp://%s:8554/ch0",serverip);
        sprintf(rtspURL_audio_eth0,"rtsp://%s:8559/testStream",serverip);
#endif
	}
	
	if(netInterface == INTERFACE_WLAN0)
	{
		ret = pthread_create(&wifiHandle, NULL, wifi_handle, NULL);
		if (ret) {
			log_err("Failed to Create wifiHandle Thread\n");
			reboot1();
			return ret;
		}
	}
#endif

#ifdef APP_CODE
#if 1
	ret = pthread_create(&app_rx_handler, NULL, app_rx_main, NULL);
	if (ret) {
		log_err("Failed to Create rtspOpen Thread\n");
		return ret;
	}
#endif
#if 1
	ret = pthread_create(&app_rx_signal_ch_handler, NULL, app_rx_signal_ch_main, NULL);
	if (ret) {
		log_err("Failed to Create rtspOpen Thread\n");
		return ret;
	}
#endif
#if 1
	ret = pthread_create(&app_rx_data_ch_handler, NULL, app_rx_data_ch_main, NULL);
	if (ret) {
		log_err("Failed to Create rtspOpen Thread\n");
		return ret;
	}
#endif
#endif

	//while (1) sleep(1);
	
	ret = pthread_create(&iHandle, NULL, MDEV_Input_ThreadFunc, (void *)im_devman);
	if (ret) {
		log_err("Failed to MDEV_Input_ThreadFunc Thread\n"); 
		reboot1();
		return ret;
	}
	
#ifdef APP_RTP

#if 1
	ret = pthread_create(&checkWr_handler, NULL, check_wr_thread, NULL);
	if (ret) {
		printf("Failed to create audio check wr thread, %d\n", ret);
		return ret;
	}
#endif

#if 1
	ret = pthread_create(&app_igmp_report_handler, NULL, app_igmp_report, NULL);
	if (ret) {
		log_err("Failed to Create rtspOpen Thread\n");
		return ret;
		}	
#endif

#if 1
	ret = pthread_create(&app_rtp_main_handler, NULL, app_rtp_main, NULL);
	if (ret) {
		log_err("Failed to Create rtspOpen Thread\n");
		return ret;
	}	
#endif

#endif

#if 0
	ret = pthread_create(&audio_handle, NULL, play_audio, NULL);
	if (ret) {
		log_err("Failed to Create play_audio Thread\n");
		return ret;
		}	
#endif
	//osd_display_init();
	//osd_sysctl_config();
#if 0
#ifdef APP_CODE
	sleep(1); //wait signal state update
	if (1 == signal_connect_state)
	{
#endif
	//printf_log("open rtsp");
#ifdef VIDEO_SUPPORT
	ret = pthread_create(&rtspHandle_video, NULL, rtspOpen_video, argv[0]);
	if (ret) {
		log_err("Failed to Create video rtspOpen Thread\n");
		reboot1();
		return ret;
	}
#endif
#ifdef AUDIO_SUPPORT
	ret = pthread_create(&rtspHandle_audio, NULL, rtspOpen_audio, argv[0]);
	if (ret) {
		log_err("Failed to Create audio rtspOpen Thread\n");
		reboot1();
		return ret;
	}
#endif
#ifdef APP_CODE
	}
#endif

#endif

#if 0
	signal(SIGINT, signalHandle);
	signal(SIGTERM, signalHandle);
	signal(SIGIO, signalioHandle);
#endif
	//~ sleep(4);
	//~ HDMI_HotPlug();
	while (1)
	{
		static unsigned int timeOutReset = 0;
		//printf("timeOutReset : %d \n", timeOutReset);
		sleep(1);
		/*
		timeOutReset++;
		if (timeOutReset > 43200)
		//if (timeOutReset > 120)
		{
			reboot1();
			printf("time out reset \n");
		}
		*/
	}
	
#if 0
	while(1)
	{
		sleep(1);
		timeoutCnt++;
		printf("timeoutCnt = %d \n", timeoutCnt);
		//printf("MAX_RTSP_TIMEOUT : %d \n", MAX_RTSP_TIMEOUT);
#ifdef APP_CODE
		gbBandwidthDetectMode = SL_TRUE;
		//printf("signal_connect_state = %d \n", signal_connect_state);
		//printf("rtspHandle_video = %d \n", rtspHandle_video);
		//printf("rtspHandle_audio = %d \n", rtspHandle_audio);
		//printf("argv[0] = %d \n", argv[0]);
		
		if(1 == signal_connect_state) //signal is connect
		{
			if(timeoutCnt >= MAX_RTSP_TIMEOUT){
			//if(timeoutCnt >= 8){
				HDMI_light_off();
#else
#ifdef WEB_ENABLE
		if(timeoutCnt >= MAX_RTSP_TIMEOUT || 1 == web_flag){
			web_flag = 0;
			//sleep(1);
#else
		if(timeoutCnt >= MAX_RTSP_TIMEOUT){
#endif
#endif
			//wpa_need_restart = 1;
			printf("timeoutCnt:%d\n",timeoutCnt);

#if 0
			state = rtsp_getState();
			switch(state)
			{
				case SERVER_NOT_CONNECT: 
					//process_osd_text_solid(10, 10, server_running);
					process_osd_text_solid(10, 10, server_not_connect);
					g_server_connect = 0;
					printf("SERVER_NOT_CONNECT\n");
					break;

				case SERVER_CONNECT: 
					//process_osd_text_solid(10, 10, server_connect);
					printf("SERVER_CONNECT\n");
					//reboot1();
					break;

				case SERVER_NOT_RUNNING: 
					process_osd_text_solid(10, 10, server_not_running);
					printf("SERVER_NOT_RUNNING\n");
					//reboot1();
					break;

				case SERVER_RUNNING: 
					printf("SERVER_RUNNING\n");
					g_server_connect = 1;
					process_osd_text_solid(10, 10, server_running);

					break;

				default:
					printf("unkonwn server state");
					process_osd_text_solid(10, 10, server_not_connect);
					break;
			}
#endif
			//Timeout is occur, so need to setup link again.

		
#ifdef VIDEO_SUPPORT
			//printf_log("rtsp_close");
			if (rtspHandle_video > 0){
				printf("rtspHandle_video : %d \n", rtspHandle_video);
			rtsp_close();
			}
#endif
#ifdef AUDIO_SUPPORT
			if (rtspHandle_audio > 0){
				printf("rtspHandle_audio : %d \n", rtspHandle_audio);
			rtsp_audio_close();
			}
#endif
			need_feed_dog = 0;
#if 1
#ifdef VIDEO_SUPPORT
			if (rtspHandle_video > 0){
			pthread_join(rtspHandle_video, &join_ret); //FIXME
			rtspHandle_video = 0;
			printf("rtspHandle_video : %d \n", rtspHandle_video);
			}
#endif

#ifdef AUDIO_SUPPORT
			if (rtspHandle_audio > 0){
			pthread_join(rtspHandle_audio, &join_ret1); //FIXME
			rtspHandle_audio = 0;
			printf("rtspHandle_audio : %d \n", rtspHandle_audio);
			}
#endif
#else
			ret = pthread_kill(rtspHandle,SIGQUIT);
			if(ret == ESRCH)
				printf("thread non-exsistent or has been killed\n");
			else if(ret == EINVAL)
				printf("signal is invalid\n");
			else
				printf("thread exsist\n");
#endif
			need_feed_dog = 1;
			printf("need_feed_dog : %d \n", need_feed_dog);
#if 1
#ifndef HANG_PAI
			if(netInterface == INTERFACE_WLAN0)
			{
				wifi_sta_stop_wpa();
				wifi_sta_stop_udhcpc();
				sleep(2);
				ret = wifi_sta_start_wpa();
				if(ret)
					printf("failed to start_wpa\n");
				ret = wifi_sta_start_udhcpc();
				if(ret)
					printf("failed to start_udhcpc\n");
				sleep(11);
			} else {
				sleep(13);
				//sleep(2);
			}
#endif
#endif

#if 0
			state = SERVER_NOT_CONNECT;
			rtsp_setState(state);
#endif

#ifdef VIDEO_SUPPORT
			if (0 == rtspHandle_video){
			ret = pthread_create(&rtspHandle_video, NULL, rtspOpen_video, argv[0]);
			if (ret) {
				log_err("Failed to Create video rtspOpen Thread\n");
				reboot1();
				return ret;
			}
			}
#endif
#ifdef AUDIO_SUPPORT
			if (0 == rtspHandle_audio){
			ret = pthread_create(&rtspHandle_audio, NULL, rtspOpen_audio, argv[0]);
			if (ret) {
				log_err("Failed to Create audio rtspOpen Thread\n");
				reboot1();
				return ret;
			}
			}
#endif
			//printf_log("open rtsp");
#if 1
			sleep(3); //must sleep here TODO when server not inserted hdmi signal
			g_cnt = 0;
			if(timeoutCnt < MAX_RTSP_TIMEOUT)
				continue;
			state = rtsp_getState();
			switch(state)
			{
				case SERVER_NOT_CONNECT: 
					//process_osd_text_solid(10, 10, server_running);
					process_osd_text_solid(10, 10, server_not_connect);
					g_server_connect = 0;
					printf("SERVER_NOT_CONNECT\n");
					timeoutCnt = MAX_RTSP_TIMEOUT;
					break;

				case SERVER_CONNECT: 
					//process_osd_text_solid(10, 10, server_connect);
					printf("SERVER_CONNECT\n");
					timeoutCnt = MAX_RTSP_TIMEOUT;
					//reboot1();
					break;

				case SERVER_NOT_RUNNING: 
					process_osd_text_solid(10, 10, server_not_running);
					printf("SERVER_NOT_RUNNING\n");
					timeoutCnt = MAX_RTSP_TIMEOUT;
					//reboot1();
					break;

				case SERVER_RUNNING: 
					printf("SERVER_RUNNING\n");
					g_server_connect = 1;
					timeoutCnt = 0;
					process_osd_text_solid(10, 10, server_running);
					break;

				default:
					printf("unkonwn server state");
					process_osd_text_solid(10, 10, server_not_connect);
					break;
			}
#endif
			g_cnt = 0;
			//timeoutCnt = 0;
		}
#ifdef APP_CODE
		}
		else //signal not connect
		{
			signal_light_flash();
			HDMI_light_off();
			g_server_connect = 0;
			printf("server not connect \n");
			process_osd_text_solid(10, 10, server_not_connect);
			//process_osd_text_overlay(10, 10, server_not_connect);
			//sleep(1);
		}
#endif
	}
#endif

	return 0;
}
