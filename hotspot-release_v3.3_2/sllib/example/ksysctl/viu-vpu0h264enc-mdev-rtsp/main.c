#include "main.h"
#include "../version.h"
#include "sl_watchdog.h"
#include "list_handler.h"
#include "audio_ioctl.h"
#include "cfginfo.h"
#include "ring_buffer.h"

#ifdef BACKUP
#include "app_hot_backup.h"
#endif /* BACKUP */
#ifdef VIDEO_IN
#include "drv_sii9293.h"
#endif /* VIDEO_IN */
#ifdef APP_RTP
#include "app_rtp_tx.h"
#endif /* APP_RTP */
#ifdef BROAD_CONTROL
#include "app_tx_broadcast.h"
#include "app_tx_ask.h"
#endif /* BROAD_CONTROL */
#ifdef WATCHDOG_UART
#include "uart_watchdog.h"
#endif /* WATCHDOG_UART */
#include "app_tx_io_ctl.h"
#ifdef WEB_ENABLE
#include "sharemem.h"
#endif /* WEB_ENABLE */
#ifdef IR_ENABLE
#include "IR.h"
#endif /* IR_ENABLE */


/*****************************thread handle******************************/
#ifdef WEB_ENABLE
static pthread_t ConfigHandle;
#endif /* WEB_ENABLE */
#ifdef IR_ENABLE
static pthread_t get_ir_handle;
#endif /* IR_ENABLE */
#ifdef VIDEO_IN
static pthread_t chip_handle;
static pthread_t pushMdev2List_handle;
#endif /* VIDEO_IN */
#ifdef WATCHDOG
static pthread_t watchdogHandle;
#endif /* WATCHDOG */
static pthread_t PullFromList_handle;
#ifdef BROAD_CONTROL
static pthread_t controlHandle;
static pthread_t IP_report_handle;
#endif /* BROAD_CONTROL */
#ifdef WATCHDOG_UART
static pthread_t uartWatchdogHandle;
#endif /* WATCHDOG_UART */
#ifdef BACKUP
static pthread_t hostBackupHandle;
static pthread_t slaveBackupHandle;
#endif /* BACKUP */
#ifdef AUDIO_IN
static pthread_t audio_handle;
#endif /* AUDIO_IN */
#ifdef DIGIT_LED
static pthread_t app_tx_io_ctl_handler;
#endif
/******************************global  variable*********************************/
int key=0;
LIST_BUFFER_S *list;
char web_flag;
char multicast[20] = "239.255.42.1";

extern CFG_INFO_S cfginfo;
static volatile int liveTimeCount = 0;
extern  int rdfd_dsp;
extern  int fd_w;
extern  int audio_fd;
extern audio_dma_info_s audio_dma_in;
extern audio_dma_info_s audio_dma_out;
extern int audio_configed;
extern sii9293_chip_s *chip;
extern int audioIn_trigger;
extern int audioOut_trigger;

/***************************************************************/
pthread_mutex_t mutex_iic;

#ifdef IR_ENABLE
typedef struct
{
    unsigned char num[IR_DATA_LENGTH];
}Node;
#endif /* IR_ENABLE */

static chanVideoPara_s cvpIn;
static chanVideoPara_s cvp;
static SLVIU_Cfg_s viu_cfg; 
static SLVENC_Cfg_s vencCfg;

#ifdef VIU_DISPLAY
static SLVPP_PV_Cfg_s pv_cfg; 
static SL_U32 pv_dev;
#endif /* VIU_DISPLAY */

static SL_U32 viu0_devman;
static SL_U32 venc_dev;
static SL_U32 vpre_dev;
static SL_U32 om_devman;
#ifdef IR_ENABLE
static SL_U32 dsp_dev;
#endif /* IR_ENABLE */

volatile int viu_started = 0;
volatile int viu_configed = 0;
H264_APPEND_INFO_s h264_append_info;
static SL_U32 need_feed_dog = 1;

//static SL_S32 g_bitrate = 40000;//20M cpu high;
//static SL_S32 g_bitrate = 20000;//20M cpu high;
//static SL_S32 g_bitrate = 16000;//16M quality is good;
//static SL_S32 g_bitrate = 12000;//12M is good;
//static SL_S32 g_bitrate = 10000;//10M is good;
static SL_S32 g_bitrate = 8000;//8M is good;
//static SL_S32 g_bitrate = 6000;//
//static SL_S32 g_bitrate = 5000;//bad kadun;
//static SL_S32 g_bitrate = 2000;//
//static SL_S32 g_bitrate = 4000;//
//static SL_S32 g_bitrate = 1000;//

SL_S32 bad_network = 0; //for eric

void *mutexlock;

int fd_config;
/********************************************************************/

typedef struct
{
	unsigned int addr;
	unsigned int data;
}SILANWORDR;

int reboot1(void)
{
	int fd;
	int res = 0;
	SILANWORDR data;

	//system("cd /;umount /user");
	//system("cd /;umount /debug");

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

	printf("\n****reboot now****\n");

	return 0;
}

SL_S32 append_h264_info(unsigned char * buf)
{
	H264_APPEND_INFO_s *append_info;

	append_info = (H264_APPEND_INFO_s *)buf;
	memcpy(append_info,&h264_append_info, sizeof(H264_APPEND_INFO_s));

	return (sizeof(H264_APPEND_INFO_s)); 
}

int flushMdev(void)
{
	void *buf;
	SL_U32 dlen;
	SL_S32 ret;
	int cnt = 20; 

	while(cnt--)
	{
		log_debug("Get a data ready block for output MDEV");
		ret = SLMDEV_getBlockRead(om_devman, &buf, &dlen, 10);
		if (SL_NO_ERROR != ret) {
			printf("SLMDEV_getBlockRead failed\n");
			break;
		}

		log_debug("Return the block");
		ret = SLMDEV_returnBlockRead(om_devman, buf);
		if (SL_NO_ERROR != ret) {
			log_err("SLMDEV_returnBlockRead");
			log_err("%d reboot",__LINE__);
			reboot1();
			return -1;
		}
	}

	return 0;
}

SL_POINTER pushMdev2List(SL_POINTER arg)
{
	void *buf;
	SL_U32 dlen;
	SL_S32 ret;
	SLVENC_ExtendStream_info_s *stream;
	SL_U32 append_size;
	//int i;

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );
	static SL_S32 restartCount = 0;

	list = list_init();
	if(!list)
	{
		log_err("fail to create list\n");
		return 0;
	}

	while(1)
	{
		if(!viu_configed)
		{
			usleep(50000);
			continue;
		}
		if(!viu_started)
		{
			ret = SLSYSCTL_prepProcess(viu0_devman);
			if (SL_NO_ERROR != ret) {
				printf("VIU0 prepProcess failed\n");
				continue;
			}

			ret = SLSYSCTL_startProcess(viu0_devman);
			if (SL_NO_ERROR != ret) {
				log_err("VIU0 startProcess failed\n");
				log_err("%d reboot",__LINE__);
				reboot1();
				continue;
			}
			viu_started = 1;
			log_note("viu started");
		}
#if 0
		if (SL_NO_ERROR != SLOS_AcquireMutex(mutexlock,SL_INFINITE))
		{
			log_err("SLOS_AcquireMutex failed\n");
			log_err("%d reboot",__LINE__);
			reboot1();
			return NULL;
		}
#endif
		ret = SLMDEV_getBlockRead(om_devman, &buf, &dlen, 100);
		if (SL_NO_ERROR != ret) {

			restartCount ++;
			printf("restartCount : %d \n", restartCount);
			if(restartCount >= 8) {
#if 0
				ret = SLSYSCTL_prepProcess(viu0_devman);
				if (SL_NO_ERROR != ret) {
					log_err("VIU0 prepProcess failed\n");
					reboot1();
					return 0;
				}

				ret = SLSYSCTL_startProcess(viu0_devman);
				if (SL_NO_ERROR != ret) {
					log_err("VIU0 startProcess failed\n");
					reboot1();
					return 0;
				}
				log_note("viu restarted");
#endif
				restartCount = 0;

			}
#if 0
			SLOS_ReleaseMutex(mutexlock);
#endif
			usleep(2000); //FIXED
			continue;
		}
		
		restartCount = 0;
		stream = (SLVENC_ExtendStream_info_s *)buf;

		if(stream->header_size)
		{
			//printf("stream->header_size:%d\n",stream->header_size);
			append_size = append_h264_info(buf + stream->stream_offset_addr + stream->stream_size);
			stream->stream_size = stream->stream_size + append_size;
			//printf("idr size:%d\n",(stream->stream_size));
			//printf("header_offset_addr:%d\n",(stream->header_offset_addr));
			//printf("stream_offset_addr:%d\n",(stream->stream_offset_addr));
			
		}
		else
		{
			//printf("p size:%d\n",stream->stream_size);
		}
#ifdef DUMP_BITRATE
		//caculate bitrate 30frames per second
		{
			static unsigned int frameCnt = 0;
			static unsigned int total_stream = 0;
			unsigned int bitrate_stream;
			total_stream += stream->stream_size;
			frameCnt ++;
			if(300 == frameCnt)
			{
				printf("total_stream:%d \n",total_stream);
				bitrate_stream = total_stream *8/10/1000000;
				printf("bitrate:%d mbits/s\n",bitrate_stream);
				total_stream = 0;
				frameCnt = 0;
			}
		}
#endif
		static char push_fail_count = 0;
		ret = list_push_data(list, buf);
		if(ret > 0)
		{
			push_fail_count++;
			printf("fail to push data,list is full\n");
			need_feed_dog = 0;
			list_flush_data(list);
			need_feed_dog = 1;
			if (push_fail_count > 20)
			{
				printf("push_fail_count > 20 , reboot\n");
				reboot1();
			}
		}
		else
		{
			push_fail_count = 0;
		}
		
		ret = SLMDEV_returnBlockRead(om_devman, (void *)buf);
		if (SL_NO_ERROR != ret) {
			log_err("SLMDEV_returnBlockRead failed\n");
			log_err("%d reboot",__LINE__);
			reboot1();
			return 0;
		}
		
#if 0
		SLOS_ReleaseMutex(mutexlock);
#endif
		usleep(1000);
		//usleep(10000);
	}

}

int PullFromMdev(unsigned char *src, int len, int channel)
{
	streamInfo_s *streamInfo = NULL;
	void *buf;
	SL_S32 ret;

	if(!list)
		return 0;

	SLVENC_ExtendStream_info_s *stream;
	if(!len) {
#if 0
		if (SL_NO_ERROR != SLOS_AcquireMutex(mutexlock,SL_INFINITE))
		{
			log_err("SLOS_AcquireMutex failed\n");
			log_err("%d reboot",__LINE__);
			reboot1();
			return -1;
		}
#endif
		list_flush_data(list);
#if 0
		if(viu_configed)
			flushMdev();
		SLOS_ReleaseMutex(mutexlock);
#endif
		return 0;
	}

	streamInfo = (streamInfo_s *)src;
	ret = list_fetch_data(list, &buf);
	if(ret)
	{
		streamInfo->ptr = 0;
		//printf("fail to fetch data\n");
		return 0;
	}

	streamInfo->ptr = (SL_U32)buf;

#if 1
	stream = (SLVENC_ExtendStream_info_s *)buf;
#if 0
	if(stream->header_size)
	{
		if(stream->header_size > 23)
		{
			log_err("stream->header_size > 23\n");
			log_err("%d reboot",__LINE__);
			reboot1();//FIXME
		}
	}
#endif
	if(stream->header_size >1000)
		printf("1 stream->header_size:%d\n",stream->header_size);
	printf("buf:%x\n",buf);
	printf("1 stream->stream_size:%d\n",stream->stream_size);
#endif
	return stream->stream_size;
}

static SL_S32 setViu0Cfg(SLVIU_Cfg_s *viu_cfg, video_info_s *video_info)
{
	SL_U32 width;
	SL_U32 height;
	viu_cfg->storeMode_cfg = PIXEL_FORMAT_YUV_PLANAR_420;

	width = video_info->width;

	if(video_info->interlaceMode)
		height = video_info->height*2;
	else
		height = video_info->height;

	viu_cfg->output_info.width = width;
	viu_cfg->output_info.height = height;
	viu_cfg->output_info.interlaceMode = video_info->interlaceMode;
	viu_cfg->output_info.input_vsync_polarity = video_info->input_vsync_polarity;
	viu_cfg->output_info.input_hsync_polarity = video_info->input_hsync_polarity;

	//if(width == 1920) //FIXME
	if(width >= 1024) //FIXME
		viu_cfg->output_info.sync_mode = SYNC_SEPARATE;
	else
		viu_cfg->output_info.sync_mode = SYNC_EMBEDDED;

#if 0
	if(viu_cfg->output_info.sync_mode == SYNC_SEPARATE)
	{
		if(video_info->interlaceMode) {
			if(1920 == width) {
				viu_cfg->output_info.HsyncHfb = 192; /* horizontal begin blank width */
				viu_cfg->output_info.VsyncOfb = 20; /*frame or interleaved odd picture's vertical begin blanking heigh */
			} else if(1280 == width) {
				viu_cfg->output_info.HsyncHfb = 260; 
				viu_cfg->output_info.VsyncOfb = 25; 
			} else if(720 == width) {
				viu_cfg->output_info.HsyncHfb = 238;
				viu_cfg->output_info.VsyncOfb = 18;
			} 
		} else { 
			if(1920 == width) {
				viu_cfg->output_info.HsyncHfb = 192;
				viu_cfg->output_info.VsyncOfb = 42;
			} else if(1280 == width) {
				if(720 == height)
				{
					viu_cfg->output_info.HsyncHfb = 260;
					viu_cfg->output_info.VsyncOfb = 25;
				} else if(800 == height) {
					viu_cfg->output_info.HsyncHfb = 200;
					viu_cfg->output_info.VsyncOfb = 28;
				}else if(960 == height) {
					viu_cfg->output_info.HsyncHfb = 400;
					viu_cfg->output_info.VsyncOfb = 39;
				}else if(1024 == height) {
					viu_cfg->output_info.HsyncHfb = 400;
					viu_cfg->output_info.VsyncOfb = 39;
				}


			} else if(720 == width) {
				viu_cfg->output_info.HsyncHfb = 244;
				viu_cfg->output_info.VsyncOfb = 36;
			} else if(752 == width) {
				if(460 == height) {
					viu_cfg->output_info.HsyncHfb = 94;
					viu_cfg->output_info.VsyncOfb = 16;
				}
			} else if(704 == width) {
				//FIXME
				viu_cfg->output_info.HsyncHfb = 244;
				viu_cfg->output_info.VsyncOfb = 36;
			}
			else if(1680 == width) 
			{
				viu_cfg->output_info.HsyncHfb = 250;
				viu_cfg->output_info.VsyncOfb = 36; //VID_TOTAL
			}
			else if(1440 == width) 
			{
				viu_cfg->output_info.HsyncHfb = 200;
				viu_cfg->output_info.VsyncOfb = 31; //VID_TOTAL
			}
			else if(1400 == width) 
			{
				viu_cfg->output_info.HsyncHfb = 200;
				viu_cfg->output_info.VsyncOfb = 36; //VID_TOTAL
			}
			else if(1360 == width) 
			{
				//bad
				viu_cfg->output_info.HsyncHfb = 500;
				viu_cfg->output_info.VsyncOfb = 9; //VID_TOTAL
			}
			else if(1024 == width) 
			{
				//1024x768
				viu_cfg->output_info.HsyncHfb = 260;
				viu_cfg->output_info.VsyncOfb = 31; //VID_TOTAL
			}

		}

	}
#else
	if(viu_cfg->output_info.sync_mode == SYNC_SEPARATE)
	{
		if(video_info->interlaceMode) {
			if(1920 == width) {
				//viu_cfg->output_info.HsyncHfb = 192; /* horizontal begin blank width */
				viu_cfg->output_info.HsyncHfb = 192;
				viu_cfg->output_info.VsyncOfb = 20; /*frame or interleaved odd picture's vertical begin blanking heigh */
			} else if(1280 == width) {
				viu_cfg->output_info.HsyncHfb = 260; 
				viu_cfg->output_info.VsyncOfb = 25; 
			} else if(720 == width) {
				viu_cfg->output_info.HsyncHfb = 238;
				viu_cfg->output_info.VsyncOfb = 18;
			} 
		} else { 
			if(1920 == width) {
				//viu_cfg->output_info.HsyncHfb = 192;
				//viu_cfg->output_info.VsyncOfb = video_info->vtavl;
				viu_cfg->output_info.HsyncHfb = 192;//188;//178;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl;//20;
				//printf("****************VsyncOfb : %d *****************\n", video_info->vtavl);
			} else if(1280 == width) {
				if(720 == height)
				{
					viu_cfg->output_info.HsyncHfb = 260;
					viu_cfg->output_info.VsyncOfb = video_info->vtavl;
				} else if(800 == height) {
					viu_cfg->output_info.HsyncHfb = 200;
					viu_cfg->output_info.VsyncOfb = video_info->vtavl;
				}else if(960 == height) {
					viu_cfg->output_info.HsyncHfb = 400;
					viu_cfg->output_info.VsyncOfb = video_info->vtavl;
				}else if(1024 == height) {
					viu_cfg->output_info.HsyncHfb = 400;
					viu_cfg->output_info.VsyncOfb = video_info->vtavl;
				}


			} else if(720 == width) {
				viu_cfg->output_info.HsyncHfb = 244;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl;
			} else if(752 == width) {
				if(460 == height) {
					viu_cfg->output_info.HsyncHfb = 94;
					viu_cfg->output_info.VsyncOfb = video_info->vtavl;
				}
			} else if(704 == width) {
				//FIXME
				viu_cfg->output_info.HsyncHfb = 244;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl;
			}
			else if(1680 == width) 
			{
				viu_cfg->output_info.HsyncHfb = 250;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl; //VID_TOTAL
			}
			else if(1440 == width) 
			{
				viu_cfg->output_info.HsyncHfb = 200;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl; //VID_TOTAL
			}
			else if(1400 == width) 
			{
				viu_cfg->output_info.HsyncHfb = 200;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl; //VID_TOTAL
			}
			else if(1360 == width) 
			{
				//bad FIXME
				viu_cfg->output_info.HsyncHfb = 500;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl; //VID_TOTAL
			}
			else if(1024 == width) 
			{
				//1024x768
				viu_cfg->output_info.HsyncHfb = 260;
				viu_cfg->output_info.VsyncOfb = video_info->vtavl; //VID_TOTAL
			}

		}

	}


#endif
	printf("\n\n***viu_cfg->output_info.HsyncHfb:%d\n",viu_cfg->output_info.HsyncHfb);
	printf("viu_cfg->output_info.VsyncOfb:%d\n\n",viu_cfg->output_info.VsyncOfb);


	//if(width >= 1280) //FIXME
	if(width >= 1024) //FIXME
		viu_cfg->output_info.yc_mode = YC_SEPARATE;
	else
		viu_cfg->output_info.yc_mode = YC_COMPOSITE;

	viu_cfg->win_info_cfg.crop_startx = 0;
	viu_cfg->win_info_cfg.crop_starty = 0;

	viu_cfg->win_info_cfg.crop_width = width;
	viu_cfg->win_info_cfg.crop_height = height;
	viu_cfg->win_info_cfg.des_width = width;
	viu_cfg->win_info_cfg.des_height = height;
	viu_cfg->mask_info_cfg.mask_attrs[0].index = 0;
	viu_cfg->mask_info_cfg.mask_attrs[0].enable = 0;
	viu_cfg->mask_info_cfg.mask_attrs[0].startX= 0;
	viu_cfg->mask_info_cfg.mask_attrs[0].startY = 0;
	viu_cfg->mask_info_cfg.mask_attrs[0].width = 100;
	viu_cfg->mask_info_cfg.mask_attrs[0].height = 100;
	viu_cfg->mask_info_cfg.mask_attrs[0].color = 0xff;

	viu_cfg->sym_info_cfg.mirror = 0;
	viu_cfg->sym_info_cfg.flip = 0;

	viu_cfg->data_yuvseq_cfg = VIU_INPUT_DATA_YUYV;//9293 order
	//viu_cfg->data_yuvseq_cfg = VIU_INPUT_DATA_VYUY;
	//viu_cfg->data_yuvseq_cfg = VIU_INPUT_DATA_YVYU;
	//viu_cfg->data_yuvseq_cfg = VIU_INPUT_DATA_UYVY;

	viu_cfg->bt656_fixcode_cfg = BT656_FIXCODE_1 ;
	viu_cfg->bt656_field_neg_cfg = BT656_FIELD_NEG_STD;
	viu_cfg->capsel_cfg = VIU_CAPSEL_BOTH;
	viu_cfg->srcFrameRate = video_info->frameRate;
	if(video_info->frameRate >= 50)
		viu_cfg->dstFrameRate = video_info->frameRate/2;
	else
		viu_cfg->dstFrameRate = video_info->frameRate;

	return 0;
}

static SL_S32 vpre_setMainLayer(SL_U32 vpre_dev, SL_S32 chnId)
{
    SLVPRE_MainLayer_s mainLayer;

    mainLayer.chnId = chnId;
    mainLayer.width = cvpIn.reso_wi;
    mainLayer.height = cvpIn.reso_hi;
    mainLayer.InWindow.startX = 0;  
    mainLayer.InWindow.startY = 0;
    mainLayer.InWindow.width = cvpIn.reso_wi;
    mainLayer.InWindow.height = cvpIn.reso_hi;
    mainLayer.OutWindow.startX = 0;
    mainLayer.OutWindow.startY = 0;
    mainLayer.OutWindow.width = cvp.reso_wi;
    mainLayer.OutWindow.height = cvp.reso_hi;
    mainLayer.sampleMode = VPreSamplingMode_420;
    mainLayer.colorMode = VPreColorMode_VideoNonInterlace;

    return SLVPRE_configMainLayer(vpre_dev, &mainLayer);
}

static SL_S32 vpre_setOverlay(SL_U32 vpre_dev, SL_S32 chnId)
{
    SLVPRE_Overlay_s overlay;

    overlay.chnId = chnId;
    overlay.enable = 0;
    overlay.format = VPreColorFormat_32BPP_8888;
    overlay.mode = VPreColorMode_TrueColor;
    overlay.space = VPreColorSpace_RGB_601_16_235;
    overlay.alpha = 0x80;
    overlay.width = cvpIn.reso_wi;
    overlay.height = cvpIn.reso_hi;
    overlay.InWindow.startX = 0;  
    overlay.InWindow.startY = 0;
    overlay.InWindow.width = cvpIn.reso_wi;
    overlay.InWindow.height = cvpIn.reso_hi;

    overlay.OutWindow.startX = 0;
    overlay.OutWindow.startY = 0;
    overlay.OutWindow.width = cvp.reso_wi;
    overlay.OutWindow.height = cvp.reso_hi;

    return SLVPRE_configOverlay(vpre_dev, &overlay);
}

static SL_S32 vpre_setBackground(SL_U32 vpre_dev, SL_S32 chnId)
{
    SLVPRE_Background_s background;

    background.chnId = chnId;
    background.mainLay_enable = 0;
    background.overlay_enable = 0;
    background.r = 0x55;
    background.g = 0x55;
    background.b = 0x55;
    
    return SLVPRE_configBackground(vpre_dev, &background);
}

static SL_S32 vpre_setEnhance(SL_U32 vpre_dev, SL_S32 chnId)
{
    SLVPRE_Enhance_s enhance;

    enhance.chnId = chnId;
    enhance.hue = 50;
    enhance.brightness = 50;
    enhance.contrast = 50;
    enhance.sat = 50;
    enhance.sharp = 50;

    return SLVPRE_configEnhance(vpre_dev, &enhance);
}

static SL_S32 vpre_setMixerOut(SL_U32 vpre_dev, SL_S32 chnId)
{
    SLVPRE_MixerOut_s mixerOut;

    mixerOut.chnId = chnId;
    mixerOut.width= cvp.reso_wi;
    mixerOut.height= BYTES_ALIGN(cvp.reso_hi,16);

    if(cvp.format == FORMAT_YUV_420)
        mixerOut.samplingMode =VPreSamplingMode_420;
    else if(cvp.format == FORMAT_YUV_444)
        mixerOut.samplingMode =VPreSamplingMode_444;

    return SLVPRE_configMixerOut(vpre_dev, &mixerOut);
}

static SL_S32 SLVENC_setCfg(SLVENC_Cfg_s *cfg, video_info_s *video_info)
{
	memset(cfg,0x00,sizeof(SLVENC_Cfg_s));
	cfg->chnNum = 1;
	cfg->chnParam[0].cfg_type = SET_ALL;
	cfg->chnParam[0].format = 0;
	cfg->chnParam[0].picWidth = cvp.reso_wi;
	cfg->chnParam[0].picQp = 32;//28;
	//cfg->chnParam[0].frameSkipDisable = 0; //discard frame
	cfg->chnParam[0].frameSkipDisable = 1; //need not discard frame

	cfg->chnParam[0].picHeight = cvp.reso_hi;

	cfg->chnParam[0].picHeight = (cfg->chnParam[0].picHeight + 15)&(~15);

	if(video_info->frameRate >= 50)
		cfg->chnParam[0].frameRateInfo = video_info->frameRate/2;
	else
		cfg->chnParam[0].frameRateInfo = video_info->frameRate;
		
#if 1
	cfg->chnParam[0].gopSize = 32; //32;
	cfg->chnParam[0].idr_interval = 32; //32;
#else
	cfg->chnParam[0].gopSize = 0;//128;//64;//320;//64;
	cfg->chnParam[0].idr_interval = 0;//128;//64;//320;//64;
#endif

#if 1
	cfg->chnParam[0].bitRate = g_bitrate;//12M
#else
	cfg->chnParam[0].bitRate = 0;
#endif
	//cfg->chnParam[0].profile = BASELINE_PROFILE;
	cfg->chnParam[0].profile = HIGH_PROFILE;
	//cfg->chnParam[0].profile = MAIN_PROFILE;
	printf("bitrate :%d\n",cfg->chnParam[0].bitRate);
	if(cfg->chnParam[0].bitRate <= 2000)
	{
		cfg->chnParam[0].IdrQp = 50; //IdrQp 16 ~ 50

		cfg->chnParam[0].maxQp = 50; //16~ 50

		cfg->chnParam[0].userQpMin = 50;//45;//35;//16;//45; //16~ 50

		cfg->chnParam[0].userMaxDeltaQp = 50;//46; //16~ 50
		cfg->chnParam[0].userMinDeltaQp = 50;//45;//35;//16;//42; //16~ 50

		cfg->chnParam[0].intraCostWeight = 0; //400

	}
	else if((cfg->chnParam[0].bitRate > 2000) && (cfg->chnParam[0].bitRate <= 6000))
	{
		cfg->chnParam[0].IdrQp = 30; //IdrQp 16 ~ 50

		cfg->chnParam[0].maxQp = 50;//35; //16~ 50

		cfg->chnParam[0].userQpMin = 40; //16~ 50

		cfg->chnParam[0].userMaxDeltaQp = 50;//40; //16~ 50
		cfg->chnParam[0].userMinDeltaQp = 40;//30;//16;//30; //16~ 50

		cfg->chnParam[0].intraCostWeight = 0; //400
	}
	else if((cfg->chnParam[0].bitRate > 6000) && (cfg->chnParam[0].bitRate <= 20000))
	{
		cfg->chnParam[0].IdrQp = 25;//26;//20;//26; //IdrQp 16 ~ 50

		cfg->chnParam[0].maxQp = 30;//30;//46; //16~ 50

		cfg->chnParam[0].userQpMin = 20;//20; //16~ 50

		cfg->chnParam[0].userMaxDeltaQp = 36;//50;//32; //16~ 50
		cfg->chnParam[0].userMinDeltaQp = 26;//16;//20; //16~ 50

		cfg->chnParam[0].intraCostWeight = 0; //400;
	}
	else
	{
		cfg->chnParam[0].IdrQp = 16;//40;//50;//45;//25;//16;//20;//20;//26; //IdrQp 16 ~ 50

		cfg->chnParam[0].maxQp = 26;//16;//24;//46; //16~ 50

		cfg->chnParam[0].userQpMin = 16;//16;//20; //16~ 50

		cfg->chnParam[0].userMaxDeltaQp = 25;//24; //16~ 50
		cfg->chnParam[0].userMinDeltaQp = 16;//16;//16;//20; //16~ 50

		cfg->chnParam[0].intraCostWeight = 0; //400;
	}

	return 0;
}

static SL_S32 setBitrate(SLVENC_Cfg_s *cfg, SL_U32 bitRate)
{
	memset(cfg,0x00,sizeof(SLVENC_Cfg_s));
	cfg->chnNum = 1;
	cfg->chnParam[0].cfg_type = SET_BITRATE;
	cfg->chnParam[0].bitRate = bitRate;
	if(bitRate <= 2000)
	{
		cfg->chnParam[0].IdrQp = 50; //IdrQp 16 ~ 50
	}
	else if ((bitRate > 2000) && (bitRate <= 6000))
	{
		cfg->chnParam[0].IdrQp = 38; //IdrQp 16 ~ 50
	}
	else if ((bitRate > 6000) && (bitRate <= 10000))
	{
		cfg->chnParam[0].IdrQp = 32;//20;//26; //IdrQp 16 ~ 50
	}
	else if ((bitRate > 10000) && (bitRate <= 14000))
	{
		cfg->chnParam[0].IdrQp = 26;//20;//26; //IdrQp 16 ~ 50
	}
	else
	{
		cfg->chnParam[0].IdrQp = 16;//16;//20;//26; //IdrQp 16 ~ 50
	}
	
	printf("new bitrate :%d\n",cfg->chnParam[0].bitRate);
	printf("new IdrQp :%d\n",cfg->chnParam[0].IdrQp);

	return 0;
}

SL_S32 SLVENC_setBitrate(SL_U32 bitRate)
{
	SL_S32 ret;
	printf("new bitrate is : %d \n", bitRate);
	setBitrate(&vencCfg, bitRate);
	ret = SLVPUENC_config(venc_dev, &vencCfg);

	return ret;
}

#ifdef VIU_DISPLAY
static void  setPvCfg(SLVPP_PV_Cfg_s *cfg, video_info_s *video_info, audio_info_s *audio_info)
{
	SL_U32 width;
	SL_U32 height;
	width = video_info->width;
	width = (width + 15)&(~15);

	if(video_info->interlaceMode)
		height = video_info->height*2;
	else
		height = video_info->height;

	cfg->sampleMode = VPPSamplingMode_420;
	cfg->colorMode = VPPColorMode_VideoNonInterlace;
	cfg->src_picWidth = width;
	cfg->src_picHeight = (height + 15)&(~15);
	cfg->picWidth = width;
	cfg->picHeight = (height + 15)&(~15);
	cfg->blkWindowMode = VPPWindowValueMode_Fixed;
	cfg->blkStartX = 0;
	cfg->blkStartY = 0;
	cfg->blkWidth = width;
	cfg->blkHeight = height;
	//cfg->blkHeight = 1080;
	printf("*************cfg->blkHeight = %d ******************\n", cfg->blkHeight);


	if(1408==cfg->blkWidth)
		cfg->blkWidth = 1392; //FIXME
#if 1
	cfg->outStartX = 0;
	cfg->outStartY = 0;
#if 1
	cfg->outWindowMode = VPPWindowValueMode_Relative;
	cfg->outWidth = 4096;
	cfg->outHeight = 4096;
#else
	cfg->outWindowMode = VPPWindowValueMode_Fixed;
	cfg->outWidth = 1920;
	cfg->outHeight = 720;//1080;
#endif
#else
	if(1408==cfg->blkWidth)
		cfg->blkWidth = 1392; //FIXME

	cfg->outWindowMode = VPPWindowValueMode_Fixed;

	if(width > 1280)
	{
		cfg->outStartX = (1920 - width)/2;
		cfg->outStartY = (1080 - height)/2;
	}
	else if(width > 720)
	{
		if(height > 720)
		{
			cfg->outStartX = (1920 - width)/2;
			cfg->outStartY = (1080 - height)/2;
		}
		else
		{
			cfg->outStartX = (1280 - width)/2;
			cfg->outStartY = (720 - height)/2;
		}
	}
	else if(width > 704)
	{
		cfg->outStartX = 0;
		cfg->outStartY = 0;
	}
	else 
	{
		cfg->outStartX = 0;
		cfg->outStartY = 0;
	}

	cfg->outWidth = width;
	if(1408==cfg->outWidth)
		cfg->outWidth = 1392; //FIXME

	cfg->outHeight = height;
#endif
	cfg->interlace = video_info->interlaceMode;

	if(video_info->interlaceMode)
		cfg->refreshRate = video_info->frameRate*2;
	else
		cfg->refreshRate = video_info->frameRate;
	//for eric
	printf("cfg->refreshRate:%d\n",cfg->refreshRate);
	printf("cfg->picWidth:%d\n",cfg->picWidth);
	printf("cfg->picHeight:%d\n",cfg->picHeight);
	printf("cfg->outWidth:%d\n",cfg->outWidth);
	printf("cfg->outHeight:%d\n",cfg->outHeight);

#if 0
	//default
	cfg->con = 50;
	cfg->bri = 50;
	cfg->hue = 0;
	cfg->sat = 50;
#else
	cfg->con = 50;//35;//50; //duibi du 35-65
	//cfg->bri = 50; //liang du
	cfg->bri = 50;//65;//50; //liang du
	cfg->hue = 0xff;//0; //se du fixed
	
	cfg->sat = 70;//65;//50;//35; //baohe du 35-65
	
#endif

	//append audio info FIXME
	cfg->fs = audio_info->fs;
	cfg->chns = audio_info->chns;
	cfg->audio_bits = audio_info->audio_bits;

}
#endif

void signalioHandle(int signo) 
{
	log_err("");
	reboot1();
}

void signalHandle(int signo)
{
	SL_S32 ret;
	log_err("");
	reboot1();
	ret = SLSYSCTL_stopProcess(viu0_devman);
	if (SL_NO_ERROR != ret) {
		log_err("VIU stop process failed");
	}
}

int trigger(void)
{
	SL_S32 ret = -1;
	video_info_s *video_info;
	audio_info_s *audio_info;
	SL_U32 hdmiMode;
	SL_U32 width;
	SL_U32 height;
	SL_U32 avmute;
	SL_U32 input_vsync_polarity; 
	SL_U32 input_hsync_polarity; 
	SL_U32 interlaceMode;
	SL_S32 need_scale_down;

	//SL_S32 bitRate_step = 20000;

	ret = SLSYSCTL_openSysCtlLib();
	if (SL_NO_ERROR != ret)
		return ret;

	video_info = &(chip->video_info);
	audio_info = &(chip->audio_info);

	while(1)
	{
		if((chip->g_video_output)&&(!viu_started)&& (!viu_configed))
		{
			width = video_info->width;
			width = (width + 15)&(~15);
			interlaceMode = video_info->interlaceMode;
			if(interlaceMode)
				height = video_info->height*2;
			else
				height = video_info->height;

			hdmiMode = video_info->hdmiMode;
			avmute = video_info->avmute;
			input_vsync_polarity = video_info->input_vsync_polarity;
			input_hsync_polarity = video_info->input_hsync_polarity;


			if(interlaceMode)
				h264_append_info.refreshRate = video_info->frameRate*2;
			else
				h264_append_info.refreshRate = video_info->frameRate; 

			h264_append_info.interlace = interlaceMode;

			if(video_info->frameRate >= 50)
				h264_append_info.frameRate = video_info->frameRate/2;
			else
				h264_append_info.frameRate = video_info->frameRate;

#if 1
			log_note("width:%d\n",width);
			log_note("height:%d\n",height);
			log_note("interlaceMode:%d\n",interlaceMode);
			log_note("frameRate:%d\n",h264_append_info.frameRate);
			log_note("hdmiMode:%d\n",hdmiMode);
			log_note("avmute:%d\n",avmute);
			log_note("input_vsync_polarity:%d\n",input_vsync_polarity);
			log_note("input_hsync_polarity:%d\n",input_hsync_polarity);
#endif
			cvpIn.chan_id = 0;
			cvpIn.io = IO_OUTPUT;
			cvpIn.format  = FORMAT_YUV_420; //yuv420
			cvpIn.reso = RESO_USER_DEFINED;
			cvpIn.reso_wi = width;
			cvpIn.reso_hi = (height + 15)&(~15);

			//if(((width == 1920) || (width == 1280))&& (bad_network)) 
			if(bad_network) 
				need_scale_down = 1;
			else
				need_scale_down = 0;

			if(need_scale_down) {
				cvp.chan_id = 0;
				cvp.io = IO_OUTPUT;
				cvp.format  = FORMAT_YUV_420; //yuv420
				cvp.reso = RESO_USER_DEFINED;
#if 1
#if 1
				cvp.reso_wi = 960;
				cvp.reso_hi = 544;
#else
				cvp.reso_wi = 720;
				cvp.reso_hi = 480;
#endif
#else
				cvp.reso_wi = cvpIn.reso_wi/2;
				cvp.reso_hi = cvpIn.reso_hi/2;
#endif
				h264_append_info.width = cvp.reso_wi; 
				h264_append_info.height = (cvp.reso_hi+ 15)&(~15);
			} else {
				cvp.chan_id = 0;
				cvp.io = IO_OUTPUT;
				cvp.format  = FORMAT_YUV_420; //yuv420
				cvp.reso = RESO_USER_DEFINED;
				cvp.reso_wi = width;
				cvp.reso_hi = (height + 15)&(~15);

				h264_append_info.width = width; 
				h264_append_info.height = height;
			}

			h264_append_info.src_width = cvpIn.reso_wi; 
			h264_append_info.src_height = cvpIn.reso_hi;

			ret = SLSYSCTL_open(&viu0_devman, ID_VIU0, &cvpIn, sizeof(cvpIn));
			if (SL_NO_ERROR != ret)
				goto close_sysctllib;

#ifdef VIU_DISPLAY
			ret = SLSYSCTL_open(&pv_dev, ID_VPP_PV, &cvpIn, sizeof(cvpIn));
			if (SL_NO_ERROR != ret)
				goto close_viu0devman;    
#endif
			if(need_scale_down) {
				ret = SLSYSCTL_open(&vpre_dev, ID_VPRE, &cvp, sizeof(cvp));
				if (SL_NO_ERROR != ret)
					goto close_viu0devman;
			}
			printf("\n**before SLSYSCTL_open(&venc_dev, ID_VPU0ENC\n");
			ret = SLSYSCTL_open(&venc_dev, ID_VPU0ENC, &cvp, sizeof(cvp));
			if (SL_NO_ERROR != ret)
				goto close_viu0devman;
			printf("\n**after SLSYSCTL_open(&venc_dev, ID_VPU0ENC\n");

			ret = SLSYSCTL_open(&om_devman, ID_MEMORY, &cvp, sizeof(cvp));
			if (SL_NO_ERROR != ret)
				goto close_venc;
#ifdef VIU_OUTPUT
			ret = SLSYSCTL_open(&viu_om_devman, ID_MEMORY, &cvpIn, sizeof(cvpIn));
			if (SL_NO_ERROR != ret)
				goto close_venc;
#endif

			if(need_scale_down) {

				ret = SLSYSCTL_bind(viu0_devman, 0, vpre_dev, 0);
				if (SL_NO_ERROR != ret)
					goto close_om_devman;

				ret = SLSYSCTL_bind(vpre_dev, 0, venc_dev, 0);
				if (SL_NO_ERROR != ret)
					goto close_om_devman;
			} else {
				ret = SLSYSCTL_bind(viu0_devman, 0, venc_dev, 0);
				if (SL_NO_ERROR != ret)
					goto close_om_devman;
			}
#ifdef VIU_OUTPUT
				ret = SLSYSCTL_bind(viu0_devman, 0, viu_om_devman, 0);
				if (SL_NO_ERROR != ret)
					goto close_om_devman;

#endif
#if 0
#ifdef VIU_DISPLAY
			ret = SLSYSCTL_bind(viu0_devman, 0, pv_dev, 0); 
			if (SL_NO_ERROR != ret)
				goto close_om_devman;

#endif
#endif
			ret = SLSYSCTL_bind(venc_dev, 0, om_devman, 0);
			if (SL_NO_ERROR != ret)
				goto unbind_venc;

			setViu0Cfg(&viu_cfg, video_info);
			ret = SLSYSCTL_setConfig(viu0_devman, &viu_cfg, sizeof(SLVIU_Cfg_s));
			if (SL_NO_ERROR != ret)
				goto unbind_om_devman;

			if(need_scale_down) {

				ret = vpre_setMainLayer(vpre_dev, 0);
				if (SL_NO_ERROR != ret)
					goto unbind_om_devman;

				ret = vpre_setOverlay(vpre_dev, 0);
				if (SL_NO_ERROR != ret)
					goto unbind_om_devman;

				ret = vpre_setBackground(vpre_dev, 0);
				if (SL_NO_ERROR != ret)
					goto unbind_om_devman;

				ret = vpre_setEnhance(vpre_dev, 0);
				if (SL_NO_ERROR != ret)
					goto unbind_om_devman;

				ret = vpre_setMixerOut(vpre_dev, 0);
				if (SL_NO_ERROR != ret)
					goto unbind_om_devman;
			}

			SLVENC_setCfg(&vencCfg, video_info);
			ret = SLVPUENC_config(venc_dev, &vencCfg);
			if (SL_NO_ERROR != ret)
				goto unbind_om_devman;

#ifdef VIU_DISPLAY
			setPvCfg(&pv_cfg, video_info, audio_info);
			ret = SLVPP_PV_config(pv_dev, &pv_cfg);
			if (SL_NO_ERROR != ret)
				goto unbind_om_devman;
#endif
			//audio_config(); //for wang 20180125
			viu_configed = 1;
		} else if((!chip->g_video_output)&&(viu_started)) {
#ifdef VIU_DISPLAY
			if(pv_dev)
			{
				printf("stop pv\n");
				SLSYSCTL_stopProcess(pv_dev);
			}
#endif
			viu_configed = 0;
			log_note("viu stopProcess\n");
			ret = SLSYSCTL_stopProcess(viu0_devman);
			if (SL_NO_ERROR != ret) {
				log_err("VIU0 stopProcess failed\n");
				log_err("%d reboot",__LINE__);
				reboot1();
				goto unbind_om_devman;
			}
			viu_started = 0;
			usleep(200000); //wait ksysctl process done
			flushMdev();
			ret = SLSYSCTL_waitProcessDone(om_devman, -1);
			if (SL_NO_ERROR != ret) {
				log_err("Output yuv MDEV wait process failed");
			}
			SLSYSCTL_unbindFromAll(om_devman);

			SLSYSCTL_unbindFromAll(venc_dev);

			if(need_scale_down) 
				SLSYSCTL_unbindFromAll(vpre_dev);

			SLSYSCTL_close(om_devman);

			SLSYSCTL_close(venc_dev);

			if(need_scale_down) 
				SLSYSCTL_close(vpre_dev);

			SLSYSCTL_close(viu0_devman);
		}

		//usleep(500000);//500ms
		usleep(50000);//50ms
	}

unbind_om_devman:
	SLSYSCTL_unbindFromAll(om_devman);

unbind_venc:
	SLSYSCTL_unbindFromAll(venc_dev);

	if(need_scale_down) 
		SLSYSCTL_unbindFromAll(vpre_dev);
close_om_devman:
	SLSYSCTL_close(om_devman);

close_venc:
	SLSYSCTL_close(venc_dev);

	if(need_scale_down) 
		SLSYSCTL_close(vpre_dev);
close_viu0devman:
	SLSYSCTL_close(viu0_devman);

close_sysctllib:
	SLSYSCTL_closeSysCtlLib();

	return 0;
}

#ifdef AUDIO_IN
SL_POINTER  audio_transfer(SL_POINTER p)
{
	audio_info_s *audio_info;
	unsigned int tmp;
	struct timeval start, end;
	static int cnt = 0;
	unsigned char *src;
	unsigned char *src1;
	unsigned char *src2;
	unsigned char *dst;

	int valid_size;
	int valid_size_1;
	int valid_size_2;
	
	unsigned int audioPushFailCount = 0;
	audio_info = &(chip->audio_info);

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );
	
	while(1)
	{
		if(audio_configed > 0)
		{
#if 1
			cnt = 0;
			if(!audioIn_trigger) {
				printf("\n\n****audioIn_trigger on\n");
				//trigger
				if (ioctl(audio_fd, AUDIO_IOCTL_IN_TRIGGER, &tmp) == -1) {
					close(audio_fd);
					return NULL;
				}
				audioIn_trigger = 1;
				//usleep(10000);
				usleep(20000);
			}
#endif
			if (ioctl(audio_fd, AUDIO_IOCTL_GET_IN_WR_DMA_ADDR, &tmp) == -1) {
				close(audio_fd);
				return NULL;
			}
			audio_dma_in.wrPtr = tmp;
			//printf("\naudio_dma_in.wrPtr:%x\n",audio_dma_in.wrPtr);
			
			if(audio_dma_in.wrPtr == audio_dma_in.rdPtr)
				valid_size = 0;
			else if(audio_dma_in.wrPtr > audio_dma_in.rdPtr)
			{
#if 1
				if(!audioOut_trigger) 
				{
					tmp = audio_dma_in.wrPtr - audio_dma_in.start_addr;
					if(tmp >= 24*1400) // audio delay
						//if(tmp >= 14*1400)
						//if(tmp >= 2*1400) //read quicker then write
						//if(tmp >= 10*1400) // audio delay
						//if(tmp >= 1400) 
					{
						//printf("\naudioOut_trigger on:%d\n",tmp);
						//trigger
						if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER, &tmp) == -1) 
						{
							close(audio_fd);
							return NULL;
						}
						audioOut_trigger = 1;
					}
				}
#endif
				valid_size = audio_dma_in.wrPtr - audio_dma_in.rdPtr;
				//printf("wr-rd:%d\n",valid_size);

				src =(unsigned char *)(audio_dma_in.start_addr_va + audio_dma_in.rdPtr - audio_dma_in.start_addr);
				src1= src;
				
				//printf("valid_size : %d \n", valid_size);
#if 1
				//gettimeofday(&start, NULL);
				if (0 > RingPCMPush(src1, valid_size))
				{
					audioPushFailCount++;
					printf("push buff failed\n");
				}
				else
				{
					audioPushFailCount = 0;
				}	
#endif
			}
			else
			{
				valid_size = audio_dma_in.size + audio_dma_in.wrPtr - audio_dma_in.rdPtr;

				valid_size_1 = audio_dma_in.size + audio_dma_in.start_addr - audio_dma_in.rdPtr;

				src =(unsigned char *)(audio_dma_in.start_addr_va + audio_dma_in.rdPtr - audio_dma_in.start_addr);

				src1 = src;

				valid_size_2 = audio_dma_in.wrPtr - audio_dma_in.start_addr;
				src =(unsigned char *)(audio_dma_in.start_addr_va);
				src2 =src;
				//gettimeofday(&start, NULL);
#if 1
				if (0 > RingPCMPush(src1, valid_size_1))
				{
					audioPushFailCount++;
					printf("audio push buff failed\n");
				}

				if (0 > RingPCMPush(src2, valid_size_2))
				{
					audioPushFailCount++;
					printf("audio push buff failed\n");
				}
#endif
			}

			audio_dma_in.rdPtr += valid_size;
			if(audio_dma_in.rdPtr >= (audio_dma_in.size + audio_dma_in.start_addr))
				audio_dma_in.rdPtr = audio_dma_in.rdPtr - audio_dma_in.size;
		}
		if (audioPushFailCount > 20)
		{
			reboot1();
		}
		usleep(10000); //must sleep here or cpu will 100%
	}
	
}
#endif

//#define DUMP_AUDIO_DATA_1
int PullFromDsp(unsigned char *src, int len)
{
	int readed = 0;
#ifdef DUMP_AUDIO_DATA_1
#define DUMP_COUNT_TOTAL 1000
	static int dump_count = 0;
	char outfilename[128] = "./test.wav";
	static FILE * outfile  = NULL;
	if(!outfile)
	{
		if ((outfile = fopen(outfilename, "wb")) == NULL) {
			log_err("can't open %s",outfilename);
			return;
		}
	}
#endif

#if 0
	struct timeval start, end;
	gettimeofday(&start, NULL);
#endif

	if(rdfd_dsp >0)
		readed = read(rdfd_dsp, src, len);
#ifdef AUDIO_OUTPUT
	if(fd_w >0)
	{
#if 1
		write(fd_w, src, readed);
#endif

	}
#endif
#if 0
	gettimeofday(&end, NULL);
	if ((end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec) > 7900)
		printf("-time: %d size %d-\n", end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec, readed);
#endif

#ifdef DUMP_AUDIO_DATA_1
	if(outfile)
	{
		if(readed > 0)
		{
			fwrite(src, readed, 1, outfile);
			dump_count ++;
			if(DUMP_COUNT_TOTAL == dump_count)
			{
				fclose(outfile);
			}
		}
	}
#endif
	if (1)//(viu_started)
		return readed;
	else
		return 0;
}

static SL_POINTER  watchdog_handle(SL_POINTER Args)
{
	int fd;
	int value, count;
	char buf[] = "hello"; 

	fd = open("/dev/silan-watchdog", O_RDWR);
	if( -1 == fd)
	{
		printf("open silan-watchdog failed\n");
		return NULL;
	}

	value = 30;
	ioctl(fd, WDIOC_SETTIMEOUT, &value);
	ioctl(fd, WDIOC_GETTIMEOUT, &value);

	printf("timeout:%d\n", value);

	count = write(fd, buf, sizeof(buf)); //feed dog
	printf("success to feed dog\n");

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

void init_system(void)
{
	int ret = 0;

	GPIO_init();
	i2c_gpio_init();

#ifdef WEB_ENABLE
    InitShareMem();
#ifdef CONFIG_FLASH_TX
    AppInitCfgInfoDefault();
    ret = AppInitCfgInfoFromFile(&fd_config);
    printf("*****\n\n ret = %d \n\n****",ret);
    if (ret<0)
    {
        if (NULL!=fd_config)
            close(fd_config);
        printf("build default config.conf \n");
        AppWriteCfgInfotoFile();
        system(CP_CONFIG_00);
        system(CP_CONFIG_01);
        system(CP_CONFIG_02);
        system(CP_CONFIG_03);
        system(CP_CONFIG_04);
        system(CP_CONFIG_05);
        system(CP_CONFIG_06);
        system(CP_CONFIG_07);
        system(CP_CONFIG_08);
        system(CP_CONFIG_09);
        system(CP_CONFIG_10);
    }
    else
    {
		printf("file fp = %d \n", fd_config);
        printf("cfg get from file \n");
        close(fd_config);
    }
#endif /* CONFIG_FLASH_TX */
    printf("cfg init ok \n");    
    strcpy(multicast, share_mem->sm_eth_setting.strEthMulticast);
#endif /* WEB_ENABLE */

#ifdef SWITCH_KEY
	Init_Multicast_and_IP(); //ip and multicast set
#endif /* SWITCH_KEY */
	init_eth();
#ifdef IR_ENABLE
	init_dsp_ir();
#endif /* IR_ENABLE */

}

int main(int argc, char* argv[])
{
	SL_S32 ret = -1;
	
	printf("********************system starting***************************\n");
	printf(PRINT_VERSION);
	init_system();

#ifdef WATCHDOG
	ret = pthread_create(&watchdogHandle, NULL, watchdog_handle, NULL);
	if (ret) {
		log_err("Failed to Create watchdogHandle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* WATCHDOG */

#ifdef WATCHDOG_UART
	ret = pthread_create(&uartWatchdogHandle, NULL, uart_watchdog, NULL);
	if (ret) {
		log_err("Failed to Create uartWatchdogHandle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* WATCHDOG_UART */

#ifdef BROAD_CONTROL
	//get ip address
	static long int ip_add = -1, i;
	inet_pton(AF_INET, share_mem->sm_eth_setting.strEthIp, &ip_add);
	ip_add = ntohl(ip_add); //host
	ip_add &= 0xFF;
	printf("ip address : %d \n", ip_add);
	
	if (201 == ip_add)
	{
		ret = pthread_create(&controlHandle, NULL, control_slave, NULL);
		if (ret) 
		{
			log_err("Failed to Create controlHandle Thread\n");
			log_err("%d reboot",__LINE__);
			reboot1();
			return ret;
		}
		
#ifdef BACKUP
		ret = pthread_create(&hostBackupHandle, NULL, backup_host, NULL);
		if (ret) 
		{
			log_err("Failed to Create hostBackupHandle Thread\n");
			log_err("%d reboot",__LINE__);
			reboot1();
			return ret;
		}
#endif /* BACKUP */
	}
	else
	{
		ret = pthread_create(&IP_report_handle, NULL, IP_broadcast_report, NULL);
		if (ret) {
			log_err("Failed to Create watchdogHandle Thread\n");
			reboot1();
			return ret;
		}
	}
#ifdef BACKUP
	if (224 == ip_add)
	{
		ret = pthread_create(&slaveBackupHandle, NULL, backup_slave, NULL);
		if (ret) 
		{
			log_err("Failed to Create slaveBackupHandle Thread\n");
			log_err("%d reboot",__LINE__);
			reboot1();
			return ret;
		}
	}
#endif /* BACKUP */
#endif /* BROAD_CONTROL */



	SLOS_CreateMutex(&mutexlock);

#ifdef IR_ENABLE
	ret = pthread_create(&get_ir_handle, NULL, get_ir, NULL);
	if (ret) {
		log_err("Failed to Create get_ir Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* IR_ENABLE */

#ifdef WEB_ENABLE
	ret = pthread_create(&ConfigHandle, NULL, sharemem_handle, NULL);
	if (ret) {
		log_err("Failed to Create Config Handle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* WEB_ENABLE */

#ifdef VIDEO_IN
	ret = pthread_create(&chip_handle, NULL, sii9293_handler, NULL);
	if (ret) {
		log_err("Failed to Create chip hander Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
	ret = pthread_create(&pushMdev2List_handle, NULL, pushMdev2List, NULL);
	if (ret) {
		log_err("Failed to Create pushMdev2List_handle, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* VIDEO_IN */

#ifdef AUDIO_IN
	ret = pthread_create(&audio_handle, NULL, audio_transfer, NULL);
	if (ret) {
		log_err("Failed to Create audio_transfer Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* AUDIO_IN */

#ifdef DIGIT_LED
    ret = pthread_create(&app_tx_io_ctl_handler,NULL,app_tx_io_ctl_main,NULL);
    if(ret!=0)
    {
        log_err("Failed to Create app_tx_io_ctl_handler Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif /* DIGIT_LED */

#ifdef APP_RTP
	sleep(3);
    printf("pull from list start \n");
	ret = pthread_create(&PullFromList_handle, NULL, PullFromList, NULL);
	if (ret) {
		log_err("Failed to Create rtsp Thread, %d\n", ret);
		return ret;
	}
#endif /* APP_RTP */

#if 0
	//signal(SIGINT, signalHandle);
	//signal(SIGTERM, signalHandle);
	//signal(SIGIO, signalioHandle);
#endif
	trigger();
	pthread_mutex_destroy(&mutex_iic);
	return 0;
}
