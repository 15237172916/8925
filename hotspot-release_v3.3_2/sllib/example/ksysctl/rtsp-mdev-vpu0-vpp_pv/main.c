/* * Note: In this example, the source video data is read from a YUV file via MDEV
 *	   write, and the BSG result is dumped to a file via MDEV read.
 */
#include "main.h"
#include "../version.h"
#include "list_handler.h"
#include "sl_watchdog.h" 
#include "display.h" 
#include "osd.h" 
#include "gui.h" 
#include "cfginfo.h" 
#include "audio_ioctl.h" 
#include "app_rx_io_ctl.h"
#include "test_gpio_rx_i2c.h"
#ifdef BROAD_CONTROL
#include "app_rx_broadcast.h"
#endif /* BROAD_CONTROL */
#ifdef APP_RTP
#include "app_rtp.h"
#endif /* APP_RTP */
#ifdef WATCHDOG_UART
#include "uart_watchdog.h"
#endif /* WATCHDOG_UART */
#ifdef WEB_ENABLE
#include "sharemem.h"
#include "init.h"
#endif /* WEB_ENABLE */
#ifdef IGMP_ENABLE
#include "app_igmp.h"
#endif /* IGMP_ENABLE */
#ifdef CONFIG_EEPROM
#include "EEPROM.h"
#endif /* CONFIG_EEPROM */
#ifdef IR_ENABLE
#include "IR.h"
#endif /* IR_ENABLE */

/*************************thread hande****************************/
static pthread_t  checkWr_handle;
static pthread_t iHandle;
#ifdef APP_RTP
static pthread_t app_rtp_main_handle;
#endif /* WEB_ENABLE */
#ifdef IGMP_ENABLE
static pthread_t app_igmp_report_handle;
#endif /* IGMP_ENABLE */
#ifdef WEB_ENABLE
static pthread_t ConfigHandle;
#endif /* WEB_ENABLE */
#ifdef  SWITCH_KEY
static pthread_t IP_switch_handle;
#endif /* SWITCH_KEY */
#ifdef WATCHDOG
static pthread_t watchdogHandle;
#endif /* WATCHDOG */
#ifdef BROAD_CONTROL
static pthread_t IP_report_handle;
static pthread_t IP_recive_handle;
#endif /* BROAD_CONTROL */
#ifdef WATCHDOG_UART
static pthread_t uartWatchdogHandle;
#endif /* WATCHDOG_UART */
#ifdef IR_ENABLE
static pthread_t send_ir_handle;
#endif /* IR_ENABLE */
#ifdef KEY
static pthread_t app_rx_io_ctl_handle;
#endif /* KEY */

/******************************global  variable*********************************/

extern CFG_INFO_S cfginfo;
extern char idr_flag;
extern char g_key_display;
extern char g_display_flag;
extern char g_ipConflict_flag;
extern char g_searchTX_flag;
extern char g_checkTxInput_flag;

pthread_mutex_t mutex_iic;

#ifdef WEB_ENABLE
char web_flag;
#endif
char g_osd_state = 0;

SL_U32 g_fs;
SL_U32 g_audio_bits;
SL_U32 g_chns;

char serverip[20] = "192.168.1.3";
char multicast[20] = "239.255.42.1";
char g_Is_E2prom = 0; //0:EEPROM 1:Flash

/********************************************************************************/
volatile static int timeoutCnt = 0;

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

static SL_S32  audio_change = 0;
static SL_S32 g_discard_frame;

static SL_U8 g_refreshRate;
static SL_U8 g_interlace;

static SL_S32 g_cnt;
static SL_U32 g_osd_need_reconfig;
static SL_U32 g_osd_overlay;

static SL_U32 need_feed_dog = 1;
static LIST_BUFFER_S *list;

//static SL_POINTER join_ret;
//static SL_POINTER join_ret1;

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

//const char * show_text ="你好:hotspot";
const char * show_text ="welcome to silan";

static int process_osd_text_overlay(int x, int y, const char *text);
static int process_osd_text_solid(int x, int y, const char *text);
static int process_osd_disable(void);

extern void * check_wr_thread(void * Args);

typedef struct
{
	unsigned int addr;
	unsigned int data;
}SILANWORDR;
#define DEV_IO_NAME		"/dev/silan_testio"
int reboot1(void)
{
	int fd;
	int res = 0;
	SILANWORDR data;
	
	printf("\n----reboot1---- \n");
	sleep(1);
	while (share_mem->ucUpdateFlag) sleep(1);
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
	} 
	else 
	{
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
int wacPushFrameToMDev(unsigned char *pframe, unsigned int uiSize)
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
	static SL_S32 discard_frame_count = 0;
	static SL_S32 tmp_fs = 0;
	static SL_S32  idr_got = 0;
	static SL_S32 frameCount = 0;
	//static SL_S32 write_block_fail_count = 0;
	static SL_S32 discard_p_frame = 0;
	static struct timeval time_begin = {0};
	static struct timeval time_end = {0};
	
	SL_S32 interval;
	SL_U32 total_frame;
	float interval_second;
	
	size = uiSize;

	//printf("wac Get a Frame size = %d \n",size);
	//return 0;
	
	//while (1)
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
			if (frameCount >= total_frame)
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
					printf("g_discard_frame:%d\n",g_discard_frame);
					discard_frame_count++;
					
					if (discard_frame_count >=5)
					{
						printf("discard_frame_count >=5 \n");
						
						discard_frame_count = 0;
						
						//return 0;
					}
				}
				frameCount = 0;
				//return 0;
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
			//printf("this is I frame \n");
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
				//g_osd_state = 0;

			}
			idr_frame = 1;
			idr_got = 1;
			discard_p_frame = 0;
			//printf("*****************idr*****************");
			idr_flag = 0;
		}
		else
		{
			//printf("this is p frame \n");
			//printf(" \n ************not idr frame : %d \n", idr_flag);
			if (idr_flag) //from wang
				return 0; //idr farme not come
			
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
		{
			cpSize = size;
		}
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
		if (SL_NO_ERROR != ret)
		{
			printf("pv SLMDEV_mallocBlockWrite failed \n");
			usleep(100000);
			
			reboot1();
			//return 0;
			goto tryAgain;
		}

		//printf("allocate block ok\n");
#if 0
		//discard test
		static int test_no;
		test_no++;
		if (test_no > 100 && test_no < 1000)
		{
			memset(pframe, 0, cpSize/2);
		}
		if (test_no > 5000)
		{
			test_no = 0;
		}
#endif
		
		memcpy(buf, pframe, cpSize);

		ret = list_push_data(list, buf);
		if (ret)
		{
			printf("fail to push data\n");
			return NULL;
		}
		else
		{
			//printf("list push data ok \n");
		}
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
		if((append_info->width > 1920) || (append_info->height > 1088) || (append_info->frameRate > 60)) 
		{
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
	if (SL_NO_ERROR != ret) 
	{		
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
	SL_U32 dd;
	static SL_S32 cnt = 0;

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );
	while (1) 
	{
		//printf("g_sysctl_configed = %d \n", g_sysctl_configed);
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
			//g_server_connect = 1;
			if(!g_osd_overlay) {
				if(osd_dev) 
				{
					//printf("\n**** stop osd\n");
					SLSYSCTL_stopProcess(osd_dev);
					g_osd_state = 0;
				}
			}
			g_cnt = 1;
		}

		SLMDEV_freeBlockWrite(im_devman, buf);
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
	{
		g_osd_state = 0;
		SLSYSCTL_stopProcess(osd_dev);
	}
	return 0;
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

#ifdef WIFI_ENABLE
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
#endif

int init_system(void)
{
	int ret;

	i2c_gpio_init();
	HPD_Init();
	pthread_mutex_init(&mutex_iic, NULL);

	ret = SLSYSCTL_openSysCtlLib();
	if (SL_NO_ERROR != ret)
		return -1;

	list = list_init();
	if(!list)
	{
		log_err("fail to create list\n");
		return -1;
	}
	InitShareMem();

	strcpy(multicast, share_mem->sm_eth_setting.strEthMulticast);
	printf("multicast address : %s \n", multicast);
	init_eth();

	osd_display_init();
	osd_sysctl_config();
	process_osd_text_solid(10, 10, OSD_VERSION);

	return 0;
}

/* For usysctl testing */
int main(int argc, char* argv[])
{
	SL_S32 ret = -1, i;
	char str_tmp[50];
	//char configs[128];
	printf(PRINT_VERSION);

#ifdef WATCHDOG
	ret = pthread_create(&watchdogHandle, NULL, watchdog_handle, NULL);
	if (ret) {
		log_err("Failed to Create watchdogHandle Thread\n");
		reboot1();
		return ret;
	}
#endif /* WATCHDOG */
	init_system();
#ifdef WATCHDOG_UART
	ret = pthread_create(&uartWatchdogHandle, NULL, uart_watchdog, NULL);
	if (ret) {
		log_err("Failed to Create uartWatchdogHandle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* WATCHDOG_UART */
	
#ifdef SWITCH_KEY
	ret = pthread_create(&IP_switch_handle, NULL, IP_switch, NULL);
	if (ret) {
		log_err("Failed to Create IP_switch Thread\n");
		reboot1();
		return ret;
	}
#endif /* SWITCH_KEY */

#ifdef KEY 
	ret = pthread_create(&app_rx_io_ctl_handle, NULL, app_rx_io_ctl_main, NULL);
	if (ret) {
		log_err("Failed to Create app_rx_io_ctl_main Thread\n");
		return ret;
	}
#endif /* KEY */

	sleep(1);

#ifdef WEB_ENABLE
	process_osd_text_solid(10, 10, share_mem->sm_eth_setting.strEthIp);
#endif

#ifdef BROAD_CONTROL
	ret = pthread_create(&IP_report_handle, NULL, IP_broadcast_report, NULL);
	if (ret) {
		log_err("Failed to Create IP_broadcast_report Thread\n");
		reboot1();
		return ret;
	}
	ret = pthread_create(&IP_recive_handle, NULL, IP_broadcast_recive, NULL);
	if (ret) {
		log_err("Failed to Create IP_broadcast_recive Thread\n");
		reboot1();
		return ret;
	}
#endif

#ifdef WEB_ENABLE
	ret = pthread_create(&ConfigHandle, NULL, sharemem_handle, NULL);
	if (ret) {
		log_err("Failed to Create sharemem_handle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#ifdef IR_ENABLE
	ret = pthread_create(&send_ir_handle, NULL, send_ir, NULL);
	if (ret) {
		log_err("Failed to send_ir Thread\n");
		reboot1();
		return ret;
	}
#endif

	ret = pthread_create(&iHandle, NULL, MDEV_Input_ThreadFunc, (void *)im_devman);
	if (ret) {
		log_err("Failed to MDEV_Input_ThreadFunc Thread\n");
		reboot1();
		return ret;
	}

#ifdef APP_RTP
	ret = pthread_create(&app_rtp_main_handle, NULL, app_rtp_main, NULL);
	if (ret) {
		log_err("Failed to Create app_rtp_main Thread\n");
		reboot1();
		return ret;
	}
#endif

#ifdef IGMP_ENABLE
	ret = pthread_create(&app_igmp_report_handle, NULL, app_igmp_report, NULL);
	if (ret) {
		log_err("Failed to Create app_igmp_report Thread\n");
		reboot1();
		return ret;
		}	
#endif

#if 1
	ret = pthread_create(&checkWr_handle, NULL, check_wr_thread, NULL);
	if (ret) {
		printf("Failed to create audio check wr thread, %d\n", ret);
		reboot1();
		return ret;
	}
#endif

#if 0
	signal(SIGINT, signalHandle);
	signal(SIGTERM, signalHandle);
	signal(SIGIO, signalioHandle);
#endif

	unsigned int mul_add, ip_add, tmp_add;
	while (1)
	{
		sleep(1);
		#if 0
		printf("g_osd_state:%d \n", g_osd_state);
		printf("g_ipConflict_flag: %d\n", g_ipConflict_flag);
		printf("g_checkTxInput_flag: %d\n", g_checkTxInput_flag);
		printf("g_display_flag: %d\n", g_display_flag);
		printf("g_searchTX_flag: %d \n", g_searchTX_flag);
		#endif
		if (g_ipConflict_flag || g_checkTxInput_flag || g_searchTX_flag || g_key_display || g_display_flag || (!g_osd_state))
		{
			//get multicast address
			inet_pton(AF_INET, share_mem->sm_eth_setting.strEthMulticast, &mul_add);
			mul_add = ntohl(mul_add); //host
			mul_add &= 0xFF;

			//get ip address
			inet_pton(AF_INET, share_mem->sm_eth_setting.strEthIp, &ip_add);
			ip_add = ntohl(ip_add); //host
			ip_add &= 0xFF;

			if (g_ipConflict_flag && ((!g_osd_state) || (tmp_add != (ip_add+mul_add))))
			{
				tmp_add = ip_add + mul_add;
				sprintf(str_tmp, "IP:%s Conflict", share_mem->sm_eth_setting.strEthIp);
				process_osd_text_solid(10, 20, str_tmp);
				g_osd_state = 1;
			}
			else if (g_checkTxInput_flag && ((!g_osd_state) || (tmp_add != (ip_add+mul_add))))
			{
				tmp_add = ip_add + mul_add;
				sprintf(str_tmp, "RX:%d Check TX:%d's HDMI input signal", ip_add, mul_add);
				process_osd_text_solid(10, 20, str_tmp);
				g_osd_state = 1;
			}
			else if (g_searchTX_flag && ((!g_osd_state) || (tmp_add != (ip_add+mul_add))))
			{
				tmp_add = ip_add + mul_add;
				sprintf(str_tmp, "RX:%d Searching TX:%d device", ip_add, mul_add);
				process_osd_text_solid(10, 20, str_tmp);
				g_osd_state = 1;
			}
			else if ((g_key_display || g_display_flag) && (!g_osd_state)) //display ip and mulitcast info
			{
				sprintf(str_tmp, "IGMP:%s IP:%s", share_mem->sm_eth_setting.strEthMulticast, share_mem->sm_eth_setting.strEthIp);
				process_osd_text_solid(10, 20, str_tmp);
				g_osd_state = 1;
				if (g_key_display || g_display_flag)
				{
					sleep(5);
					g_key_display = 0;
					g_display_flag = 0;
				}
			}
		}
		else
		{
			if (1==g_osd_state)
			{
				process_osd_disable();
				g_osd_state = 0;
			}
		}
#if 0
		static FILE *stream = NULL;
		static char buf[20] = {0};
		static char HPD_flag = 0;
		stream = popen("/user/word.csky 0xbfa500DF", "r");
		fread(buf, 1, sizeof(buf), stream);
		//printf("%s", buf);
		pclose(stream);
		//printf("%d \n", strncmp(buf, "0x00b000c0", 10));
		if (0 == strncmp(buf, "0x00b000c0", 10))
		{
			if (0 == HPD_flag)
			{
				Reset_HPD();
				HPD_flag = 1;
				printf("\n--------------------------\n");
			}
		}
		else
		{
			Reset_HPD();
		}
#endif
	}

	pthread_mutex_destroy(&mutex_iic);
	return 0;
}
