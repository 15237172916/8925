#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <sys/mman.h>
#include <sl_types.h>
#include <sl_error.h>
#include <sl_api_filedev.h>
#include <sl_api_ksysctl.h>
#include <sl_api_memdev.h>
#include <sl_api_viu.h>
#include <sl_param_vcodec.h>
#include <sl_api_vcodec.h>
#include <vpp/sl_param_pv.h>
#include <vpp/sl_api_pv.h>
#include <sl_api_vpre.h>
#include <sl_param_vpre.h>
#include <pthread.h>
#include <sl_debug.h>
#include <sl_mmutil.h>
#include <sl_file.h>
#include <sl_rtsp.h>
#include <linux/soundcard.h>
#include <errno.h>
//#include "drv_sii9293.h"
#include "sl_watchdog.h"
#include "list_handler.h"
#include "audio_ioctl.h"
#include "cfginfo.h"
#include "wifi_ap.h"
#include "ring_buffer.h"
#include <sys/time.h>
#if 1
//#include "drv_sii9293_register.h"
#include "drv_sii9293.h"
#endif
#include "app_rtp_tx.h"
#include "../version.h"

#if 1
#define IR_DATA_LENGTH 2040
#define IR_DATA_NUM 2
#else
#define IR_DATA_LENGTH 1360
#define IR_DATA_NUM 3
#endif

//#define IR_DEBUG
//#define ENABLE_GET_IR


#ifdef ENABLE_GET_IR
#define IR_SERVER_PORT    7998
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024

typedef struct
{
    unsigned char num[IR_DATA_LENGTH];
}Node;
#endif

#define WIDTH 1920
#define HEIGHT 1080

#define VIU_DISPLAY
//#define VIU_OUTPUT
#define VIU_OUT_FRAMES 1
//#define H264_OUTPUT
#define WEB_ENABLE
//#define APP_CODE
//#define KVM_UART
#define DEBUG_OFF
#define APP_IO
#define APP_RTP
//#define  SWIT_MULTICAST


#ifdef APP_CODE
#include "app_tx_signal_ch.h"
#include "app_tx_data_ch.h"
#include "app_tx_io_ctl.h"

//static pthread_t server_broadcast_handler;
static pthread_t app_tx_handler;
static pthread_t app_tx_light_ctl_handler;

static pthread_t app_tx_signal_ch_handler_a;
static pthread_t app_tx_data_ch_handler_a;

static pthread_t app_tx_signal_ch_handler_b;
static pthread_t app_tx_data_ch_handler_b;

char connect_state_a = 0;
char connect_state_b = 0;

#endif

#ifdef KVM_UART

#include "app_tx_uart.h"
static pthread_t app_tx_uart_handler;

#endif
char 	web_flag;
char multicast[20] = "239.255.42.1";
#ifdef WEB_ENABLE
#include "sharemem.h"

static pthread_t ConfigHandle;

#endif

int key=0;
LIST_BUFFER_S *list;


extern CFG_INFO_S cfginfo;
static volatile int liveTimeCount = 0;
extern  int rdfd_dsp;
extern  int fd_w;
extern  int audio_fd;
extern audio_dma_info_s audio_dma_in;
extern audio_dma_info_s audio_dma_out;
extern int audio_configed;

#define USLEEP_TIME	    5*1000
#define LIVE_TIME_SECONDS 20

static pthread_t rtspserver;
static pthread_t chip_handler;
#ifdef ENABLE_GET_IR
static pthread_t get_ir_handler;
#endif
static pthread_t noteLive_handle;
static pthread_t  pushMdev2List_handle;
static pthread_t  viu_output_handle;
static pthread_t	watchdogHandle;
static pthread_t	setBitrateHandle;
static pthread_t 	PullFromList_handler;

static pthread_t 	switch_multicast_handler;   //Jason add

#ifdef RINGBUFF
static pthread_t audio_handler;
#endif

static chanVideoPara_s cvpIn;

static chanVideoPara_s cvp;
static SLVIU_Cfg_s viu_cfg; 
static SLVENC_Cfg_s vencCfg;

#ifdef VIU_DISPLAY
static SLVPP_PV_Cfg_s pv_cfg; 
static SL_U32 pv_dev;
#endif

static SL_U32 viu0_devman;
static SL_U32 venc_dev;
static SL_U32 vpre_dev;
static SL_U32 om_devman;
#ifdef VIU_OUTPUT
static SL_U32 viu_om_devman;
#endif
#ifdef ENABLE_GET_IR
static SL_U32 dsp_dev;
#endif
extern sii9293_chip_s *chip;
volatile int viu_started = 0;
volatile int viu_configed = 0;
H264_APPEND_INFO_s h264_append_info;
//static int MdevCount = 0;

extern int audioIn_trigger;
extern int audioOut_trigger;
static SL_U32 interface;
static server_param_t server_param;
static SL_U32 need_feed_dog = 1;
//static SL_S32 g_bitrate = 40000;//20M cpu high;
//static SL_S32 g_bitrate = 20000;//20M cpu high;
//static SL_S32 g_bitrate = 16000;//16M quality is good;
//static SL_S32 g_bitrate = 12000;//12M is good;
//static SL_S32 g_bitrate = 10000;//10M is good;
static SL_S32 g_bitrate = 8000;//8M is good;
//static SL_S32 g_bitrate = 5000;//bad kadun;
//static SL_S32 g_bitrate = 2000;//
//static SL_S32 g_bitrate = 4000;//
//static SL_S32 g_bitrate = 1000;//


SL_S32 bad_network = 0; //for eric

void *mutexlock;
#define DEV_IO_NAME		"/dev/silan_testio"

int fd_config;
FILE *fp_outfile;
//#define DUMP_BITRATE

static unsigned char *dsp_ir_start_addr_va;

extern int reboot1(void);
typedef struct
{
	unsigned int addr;
	unsigned int data;
}SILANWORDR;


#ifdef APP_CODE
static SL_S32 SLVENC_setBitrate(SL_U32 bitRate);

//COMMAND g_TxCommand; //remove for link issue 2017-06-09
//SL_BOOL g_bDataTestEn=FALSE;

void  *app_tx_main(void)
{
    int ret = -1;
    
    //gbDataTestEn = SL_FALSE;
    //gbBandwidthDetectMode = SL_FALSE;
    
    while(1)
    {
		sleep(1);
        ///////////////process command///////////////
        if (SL_TRUE==g_TxCommand.bCmdFlag)
        {
            g_TxCommand.bCmdFlag = SL_FALSE;
            //printf("Get command type = %d \n", g_TxCommand.iCmdData[0]);
            
            switch(g_TxCommand.iCmdData[0])
            {
                case 1: //change bitRate
                {
                    printf("Current bitrate = %d \n", g_bitrate);
#if 0
                    if(g_bitrate > 2000)
                    {
                        g_bitrate = g_bitrate - 1000;
                    }
                    else
                    {
                        g_bitrate = 12000;
                    }
#endif
#if 1	//three level
					if (g_bitrate >= 12000)
                    {
                        g_bitrate = 4000;
                    }
                    else
                    {
                        g_bitrate += 4000;
                    }
#endif 
                    if(viu_configed)
                        ret = SLVENC_setBitrate(g_bitrate);  
                    
                    printf(" bitrate change to %d ret = %d\n",g_bitrate,ret);     
					
					if(SL_FALSE == g_RemoteCmd.bSendFlag)
					{
						printf("Set Remote Command to Send\n");
						g_RemoteCmd.bSendFlag = SL_TRUE;
						g_RemoteCmd.uCmdBuf[0]=1; //change bitrate
						g_RemoteCmd.uCmdBuf[1]=g_bitrate/1000;
					}
					break;
                }
                case 3: //TestMode Change
                {
                    //if(SL_TRUE == gbDataTestEn)
                    if(0 == g_TxCommand.iCmdData[1])
                    {
                        gbDataTestEn = SL_FALSE;
                        g_RemoteCmd.uCmdBuf[0]=3;
                        g_RemoteCmd.uCmdBuf[1]=0;
                    }
                    else
                    {
                        gbDataTestEn = SL_TRUE;
                        g_RemoteCmd.uCmdBuf[0]=3;
                        g_RemoteCmd.uCmdBuf[1]=1;
                    }
                                   
                    if(SL_FALSE == g_RemoteCmd.bSendFlag)
                    {
                        printf("Set Remote Command to Send\n");
                        g_RemoteCmd.bSendFlag = SL_TRUE;
                    }
                    break;
                }

                default:
                    break;
            }
        }
    }
}

#endif
/**********************************************************/

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
	if (fd_log < 0){
		printf("printf_log open error \n");
		perror("open");
	}
	ret = gettimeofday(&tv1,&tz);
	if (ret < 0){
		printf("printf_log gettimeofday error \n");
		perror("gettimeofday");
	}
	sprintf(buf, "%ld log: ", tv1.tv_sec-144221050);
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

	system("cd /;umount /user");
	system("cd /;umount /debug");

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
#ifdef VIU_OUTPUT
SL_POINTER viu_output(SL_POINTER arg)
{
	void *buf;
	SL_U32 dlen;
	SL_S32 ret;
	char outfilename[128] = "./test.yuv";
	FILE * outfile;
	static int cnt = 0;
	if ((outfile = fopen(outfilename, "wb")) == NULL) {
		log_err("can't open %s",outfilename);
		return;
	}

	printf("\n**viu_output***\n");
	while(1)
	{
		if(viu_started)
		{
			ret = SLMDEV_getBlockRead(viu_om_devman, &buf, &dlen, 100);
			if (SL_NO_ERROR != ret) {
				usleep(10000);
				continue;
			}
			printf("dlen:%d\n",dlen);
//#define SWAP_BITS

#ifdef SWAP_BITS
			unsigned char *pTmp;
			unsigned char *src;
			src = (unsigned char *)buf;
			pTmp = malloc(dlen);
			int i;
			int j;
			for(i=0;i<dlen;i++)
			{
		
				for(j=0;j<8;j++)
				{
				if(src[i]&(1<<j))
					pTmp[i]|=1<<(7-j);
				}
			
			}
			printf("src[i]:%x\n",src[i-1]);
			printf("pTmp[i]:%x\n",pTmp[i-1]);

			fwrite(pTmp, dlen, 1, outfile);
			free(pTmp);

#else
			fwrite(buf, dlen, 1, outfile);
#endif
			sync();
			printf("write viu done\n");
			ret = SLMDEV_returnBlockRead(viu_om_devman, (void *)buf);
			if (SL_NO_ERROR != ret) {
				log_err("SLMDEV_returnBlockRead failed\n");
				return 0;
			}
			usleep(20000);
			cnt ++ ;
			if(cnt >= VIU_OUT_FRAMES)
			{
				sync();
				return 0; 
			}
		}
	}
}
#endif
SL_POINTER pushMdev2List(SL_POINTER arg)
{
	void *buf;
	SL_U32 dlen;
	SL_S32 ret;
	SLVENC_ExtendStream_info_s *stream;
	SL_U32 append_size;
	int i;

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );
	static SL_S32 restartCount = 0;

	list = list_init();
	if(!list)
	{
		log_err("fail to create list\n");
		return 0;
	}

#ifdef H264_OUTPUT
	h264_output();
#endif
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
			
#ifdef H264_OUTPUT
			fwrite(buf+stream->header_offset_addr, stream->header_size, 1, fp_outfile);
			fwrite(buf+stream->stream_offset_addr, stream->stream_size, 1, fp_outfile);
#endif	
		}
		else
		{
#ifdef H264_OUTPUT
			fwrite(buf+stream->stream_offset_addr, stream->stream_size, 1, fp_outfile);
#endif
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
		ret = list_push_data(list, buf);
		if(ret > 0)
		{
			//printf("fail to push data,list is full\n");
			need_feed_dog = 0;
			list_flush_data(list);
			need_feed_dog = 1;
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

#if 0
typedef struct
{
    unsigned int iProbe;
	unsigned char uSeq:8;
	unsigned int  iLen:24;
	unsigned int iTimeStamp;
}DATAHEAD;


//thread
SL_POINTER  PullFromList(SL_POINTER p)
{
	void *buf;
	SL_S32 ret;
	unsigned char *dst;

	int server_socket, new_server_socket;
	struct sockaddr_in   server_addr;
	struct tcp_info info;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(8001);

	SLVENC_ExtendStream_info_s *stream;

	dst = malloc(2*1024*1024); //FIXME
	if(!dst)
		printf("fail to malloc\n");
	//flush the list, get the newest data
	if (SL_NO_ERROR != SLOS_AcquireMutex(mutexlock,SL_INFINITE))
		return NULL;

	list_flush_data(list);
	if(viu_configed)
		flushMdev();

	SLOS_ReleaseMutex(mutexlock);
	
	// create a stream socket
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		printf("Create Socket Failed!\n");
		exit(1);
	}
    printf("Create Socket OK \n");
    setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&info,sizeof(info));

    //bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Bind Port: 8001 Failed!\n");
        exit(1);
    }
    printf("bind socket ok \n");
    
//    sleep(5);
    
    // listen
    if (listen(server_socket, 20))
    {
        printf("Server Listen Failed!\n");
        exit(1);
    }
    printf("listen socket ok \n");
        
ReAccept:

	printf("Start Accept \n");
	new_server_socket = accept(server_socket, NULL,NULL);
	printf("Finish Accept \n");
	if (new_server_socket < 0)
	{
		printf("Server Accept Failed!\n");
		exit;
	}
	else
	{
		printf("Server Aceept OK \n");
	}
        
    struct timeval tv1,tv2;
    struct timezone tz;
    //struct tcp_info info;
    
    int optlen = sizeof(struct tcp_info);
    int pos,len;
    int iDataNum = 0;
    DATAHEAD DataHead;
    DataHead.iProbe = 0x1A1B1C1D;
    DataHead.uSeq = 0;

	
	while(1)
	{
		ret = list_fetch_data(list, &buf);
		if(ret)
		{
			usleep(5000);
			continue;
		}
		
//#ifdef APP_CODE
        DataHead.uSeq += 1;
//#endif
		stream = (SLVENC_ExtendStream_info_s *)buf;
		printf("stream->header_size : %d \n", stream->header_size);
		//while (1)sleep(1);
		if(stream->header_size)
		{
			if(stream->header_size > 23)
			{
				//reboot1();//FIXME
			}
			//#ifdef APP_CODE
			DataHead.iLen =  stream->header_size + stream->stream_size;
			
                len=send(new_server_socket, &DataHead, sizeof(DataHead),0);
                if(len <= 0)
                {
                    printf("Send data len <= 0 len= %d \n",len);
                    printf("errno is %d\n",errno);
                    close(new_server_socket);
                    goto ReAccept;
                }
                else
                {
                    //printf("Send data len = %d \n",len);
                 }     

			
			memcpy(dst, (unsigned char *)stream + stream->header_offset_addr, stream->header_size);
			memcpy(dst + stream->header_size, (unsigned char *)stream + stream->stream_offset_addr, stream->stream_size);
			
			int i,templen;
			templen = stream->header_size+stream->stream_size;
			/* removed debug info for speed up
			printf("stream total size is %d \n",templen);
			for(i=(templen-24);i<templen;i++)
			    printf("%02x ",dst[i]);
			*/
	        //printf("Get frame with header, size is %d",stream->header_size + stream->stream_size);
	        //gettimeofday(&tv1,&tz);			
                len=send(new_server_socket, dst, stream->header_size + stream->stream_size,0);
                if(len <= 0)
                {
                    printf("Send data len <= 0 len= %d \n",len);
                    printf("errno is %d\n",errno);
                    close(new_server_socket);
                    goto ReAccept;
                }
                else
                {
                /* removed debug info for speed up
                    printf("Frame with Header Send data len = %d \n",len);
                */
                }
            //#else
			//memcpy(dst, (unsigned char *)stream + stream->header_offset_addr, stream->header_size);
			//memcpy(dst + stream->header_size, (unsigned char *)stream + stream->stream_offset_addr, stream->stream_size);
	        //printf("Get frame with header, size is %d",stream->header_size + stream->stream_size);
            
            //#endif
                			
			//eric dst to send;
			//send(dst, stream->header_size + stream->stream_size);
			
		}
		else
		{
			//eric dst to send;
			//send((unsigned char *)stream + stream->stream_offset_addr, stream->stream_size);
            //printf("Get frame without header, size is %d",stream->stream_size);

   			//#ifdef APP_CODE
            DataHead.iLen =  stream->stream_size;
            
			len=send(new_server_socket, &DataHead, sizeof(DataHead),0);
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(new_server_socket);
				goto ReAccept;
			}
			else
			{
			   // printf("Send data len = %d \n",len);
			 }         
							
			len=send(new_server_socket, (unsigned char *)stream + stream->stream_offset_addr, stream->stream_size,0);
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(new_server_socket);
				goto ReAccept;
			}
			else
			{
			   // printf("Send data len = %d \n",len);
				if(len < 20)
				{
					
					for(pos=0;pos<len;pos++)
						printf("%02X ",*(stream+stream->stream_offset_addr+pos));
					printf("stream offset addr = %d \n",stream->stream_offset_addr);
				}
			}
	   // #else
			printf("Get frame without header, size is %d",stream->stream_size);
	   // #endif

		}

	}
	free(dst);

	return NULL;
}
#endif

#ifdef H264_OUTPUT

int h264_output(void)
{
	
	char outfilename[12] = "./test.264";
	
	if ((fp_outfile = fopen(outfilename, "wb")) == NULL)
	{
		printf("can't open %s \n", outfilename);
		return -1;
	}
	printf("open %s file ok \n", outfilename);
	
	return 0;
}
#endif

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


int PushToMdev(unsigned char *src)
{
#if 0
	SL_S32 ret;
	ret = SLMDEV_returnBlockRead(om_devman, (void *)src);
	if (SL_NO_ERROR != ret) {
		return -1;
	}

	if(MdevCount==2)
	{
		printf("pusd and pull not pare occur, exit");
		exit(-1);
	}
	MdevCount --;
#endif
	return 0;
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
	//cfg->chnParam[0].frameSkipDisable = 0; //dicard frame
	cfg->chnParam[0].frameSkipDisable = 1; //need not dicard frame

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
	else if((cfg->chnParam[0].bitRate > 6000) && (cfg->chnParam[0].bitRate <= 12000))
	{
		cfg->chnParam[0].IdrQp = 20;//26;//20;//26; //IdrQp 16 ~ 50

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

static SL_S32 SLVENC_setBitrate(SL_U32 bitRate)
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

	SL_S32 bitRate_step = 20000;

	ret = SLSYSCTL_openSysCtlLib();
	if (SL_NO_ERROR != ret)
		return ret;

	video_info = &(chip->video_info);
	audio_info = &(chip->audio_info);
	
#if 0 //video
	//eric force config. not from 9293
#if 1
	video_info->width = 1280;
	video_info->height =720;
	video_info->interlaceMode = 0;

	video_info->hdmiMode = 1;
	video_info->avmute = 0;
	video_info->input_vsync_polarity = 0;
	video_info->input_hsync_polarity = 0;

	video_info->frameRate = 60;
	chip->g_video_output = 1;
	viu_started = 0;
	viu_configed = 0;
#else
	video_info->width = 1920;
	video_info->height =1080;
	video_info->interlaceMode = 0; 

	video_info->hdmiMode = 1;
	video_info->avmute = 0;
	video_info->input_vsync_polarity = 0;
	video_info->input_hsync_polarity = 0;

	video_info->frameRate = 60;
	chip->g_video_output = 1;
	viu_started = 0;
	viu_configed = 0;
#endif
#endif

#if 0 //audio
	audio_info_s *audio_info;
	audio_info = &(chip->audio_info);
	audio_info->fs = 48000;
	chip->audio_status = AudioOn;
	chip->audio_prev_status = AudioOn;
	chip->g_audio_output = 1;
	audio_info->audio_bits = 16;
	h264_append_info.audio_bits = audio_info->audio_bits;
	h264_append_info.chns = audio_info->chns;
	
	
#endif
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
#if 0
		//g_bitrate += bitRate_step;
		if(g_bitrate == 30000)
			g_bitrate =3000;
		else
			g_bitrate =30000;
		if(viu_configed)
			SLVENC_setBitrate(g_bitrate);
		sleep(10);
#endif

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

#ifdef RINGBUFF
//#define DUMP_AUDIO_DATA
SL_POINTER  audio_transfer(SL_POINTER p)
{
	//SL_S32 ret = -1;
	audio_info_s *audio_info;
	unsigned int tmp;
	//char tmpbuf[1400];
	//int readed;
	struct timeval start, end;
	static int cnt = 0;
	unsigned char *src;
	unsigned char *src1;
	unsigned char *src2;
	unsigned char *dst;
	
	int valid_size;
	int valid_size_1;
	int valid_size_2;
	int write_size_1;
	int write_size_2;
	
	int len, i;
	
	audio_info = &(chip->audio_info);
#ifdef DUMP_AUDIO_DATA
#define DUMP_COUNT_TOTAL 10000 
	static int dump_count = 0;
	char outfilename[128] = "./test.wav";
	FILE * outfile;
	if ((outfile = fopen(outfilename, "wb")) == NULL) {
		log_err("can't open %s",outfilename);
		return;
	}
#endif
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
				
#ifdef DUMP_AUDIO_DATA
								
				if (outfile)
				{
					fwrite(src, valid_size, 1, outfile);
					dump_count ++;
					//printf("%x, %x, %x\n",audio_dma_in.start_addr_va, audio_dma_in.rdPtr, audio_dma_in.start_addr);
					//printf("rdPtr:%x,wrPtr:%x,valid_size:%d\n",audio_dma_in.rdPtr,audio_dma_in.wrPtr,valid_size);
					if(DUMP_COUNT_TOTAL == dump_count)
					{
						fclose(outfile);
						outfile = NULL;
					}
				}
#endif
#ifdef AUDIO_OUTPUT

#if 0
				key ++;
				//if(key <40)
				printf("w:%x\n",audio_dma_out.wrPtr);
				//if(key <40)
				{

					if (ioctl(audio_fd, AUDIO_IOCTL_GET_OUT_RD_DMA_ADDR, &tmp) == -1) {
						close(audio_fd);
						return -1;
					}
					audio_dma_out.rdPtr = tmp;
					printf("-r:%x\n",audio_dma_out.rdPtr);
				}
#endif

#if 0
				if((audio_dma_out.wrPtr + valid_size) < (audio_dma_out.start_addr + audio_dma_out.size))
				{
					dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);
					memcpy(dst, src, valid_size);
					audio_dma_out.wrPtr += valid_size;
				} 
				else
				{
					write_size_1 = audio_dma_out.start_addr + audio_dma_out.size - audio_dma_out.wrPtr;

					dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);

					memcpy(dst, src, write_size_1);

					write_size_2 = valid_size - write_size_1;

					dst =(unsigned char *)audio_dma_out.start_addr_va;
					src += write_size_1;
					memcpy(dst, src, write_size_2);

					audio_dma_out.wrPtr = audio_dma_out.start_addr + write_size_2;
				}

#endif
#endif

#ifdef AUDIO_OUTPUT

#if 0
				if(!audioOut_trigger) {

					tmp = audio_dma_out.wrPtr - audio_dma_out.start_addr;
					if(tmp > 1400*23) {
						//trigger
						if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER, &tmp) == -1) {
							close(audio_fd);
							return NULL;
						}
						audioOut_trigger = 1;
					}
				}
#endif
#endif
#if 0
				//gettimeofday(&start, NULL);
				if (0 > PCMPush(src1, valid_size))
				{
					//printf("push buff failed\n");
				}
#endif
#if 1
				//gettimeofday(&start, NULL);
				if (0 > RingPCMPush(src1, valid_size))
				{
					//printf("push buff failed\n");
				}
#endif

#if 0
				gettimeofday(&end, NULL);
				if((end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec) > 8000)
					printf("interval:%d %d\n",end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec,valid_size);
#endif
			}
			else
			{
				//printf("audio_dma_in.wrPtr : %d \n", audio_dma_in.wrPtr);
				//printf("audio_dma_in.rdPtr : %d \n", audio_dma_in.rdPtr);
				//printf("audio_dma_in.size : %d \n", audio_dma_in.size);
				
				valid_size = audio_dma_in.size + audio_dma_in.wrPtr - audio_dma_in.rdPtr;

				valid_size_1 = audio_dma_in.size + audio_dma_in.start_addr - audio_dma_in.rdPtr;

				src =(unsigned char *)(audio_dma_in.start_addr_va + audio_dma_in.rdPtr - audio_dma_in.start_addr);

				src1 = src;
#ifdef DUMP_AUDIO_DATA
				if(outfile)
				{
					fwrite(src, valid_size_1, 1, outfile);
					dump_count ++;
					if(DUMP_COUNT_TOTAL == dump_count)
					{
						fclose(outfile);
						outfile = NULL;
					}
				}
#endif

#ifdef AUDIO_OUTPUT
#if 0
				if((audio_dma_out.wrPtr + valid_size_1) < (audio_dma_out.start_addr + audio_dma_out.size))
				{
					dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);
					memcpy(dst, src, valid_size_1);
					audio_dma_out.wrPtr += valid_size_1;
				} 
				else
				{
					write_size_1 = audio_dma_out.start_addr + audio_dma_out.size - audio_dma_out.wrPtr;

					dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);

					memcpy(dst, src, write_size_1);

					write_size_2 = valid_size_1 - write_size_1;

					dst =(unsigned char *)audio_dma_out.start_addr_va;
					src += write_size_1;
					memcpy(dst, src, write_size_2);

					audio_dma_out.wrPtr = audio_dma_out.start_addr + write_size_2;
				}
#endif
#endif
				valid_size_2 = audio_dma_in.wrPtr - audio_dma_in.start_addr;
				src =(unsigned char *)(audio_dma_in.start_addr_va);
				src2 =src;
#ifdef DUMP_AUDIO_DATA
				if(outfile)
				{
					fwrite(src, valid_size_2, 1, outfile);
					dump_count ++;
					if(DUMP_COUNT_TOTAL == dump_count)
					{
						fclose(outfile);
						outfile = NULL;
					}
				}
#endif

#ifdef AUDIO_OUTPUT

#if 0
				if((audio_dma_out.wrPtr + valid_size_2) < (audio_dma_out.start_addr + audio_dma_out.size))
				{
					dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);
					memcpy(dst, src, valid_size_2);
					audio_dma_out.wrPtr += valid_size_2;
				} 
				else
				{
					write_size_1 = audio_dma_out.start_addr + audio_dma_out.size - audio_dma_out.wrPtr;

					dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);

					memcpy(dst, src, write_size_1);

					write_size_2 = valid_size_2 - write_size_1;

					dst =(unsigned char *)audio_dma_out.start_addr_va;
					src += write_size_1;
					memcpy(dst, src, write_size_2);

					audio_dma_out.wrPtr = audio_dma_out.start_addr + write_size_2;
				}
#endif
#endif
				//gettimeofday(&start, NULL);
#if 0
				if (0 > PCMPush(src1, valid_size_1))
				{
					//printf("push buff failed\n");
				}

				if (0 > PCMPush(src2, valid_size_2))
				{
					//printf("push buff failed\n");
				}
#endif
				//gettimeofday(&start, NULL);
#if 1
				if (0 > RingPCMPush(src1, valid_size_1))
				{
					//printf("push buff failed\n");
				}

				if (0 > RingPCMPush(src2, valid_size_2))
				{
					//printf("push buff failed\n");
				}
#endif

#if 0
				gettimeofday(&end, NULL);
				if((end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec) > 8000)
					printf("1 interval:%d %d\n",end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec,valid_size_1 + valid_size_2);
#endif
			}

			audio_dma_in.rdPtr += valid_size;
			if(audio_dma_in.rdPtr >= (audio_dma_in.size + audio_dma_in.start_addr))
				audio_dma_in.rdPtr = audio_dma_in.rdPtr - audio_dma_in.size;
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

static SL_POINTER  setBitrate_handle(SL_POINTER Args)
{
	static int key = 0;
	sleep(10);
	while(1)
	{
		sleep(20);
		if(0 == key)
		{
			SLVENC_setBitrate(2000);
			key = 1;
		} else{

			SLVENC_setBitrate(16000);
			key = 0;
		}
	}

	return NULL;
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
#if 0
				//for debug eric
				{
					if(!bad_network)
					{
						bad_network = 1;
						g_bitrate =1800;
					}
					else
					{
						bad_network = 0;
						g_bitrate =8000;
					}

					chip->video_status = Unplug;
					chip->video_prev_status = Unplug;
					chip->g_video_output = 0;

				}
#endif
			}
		}
	}

	return NULL;
}

#ifdef ENABLE_GET_IR
static int send_to_dsp(unsigned char *buf, int len)
{
	static int offset = 0;

	//while(*(unsigned char *)(dsp_ir_start_addr_va + offset))
	{
		printf("send to dsp while \n");
		usleep(10000);
	}
	memcpy((unsigned char *)(dsp_ir_start_addr_va + offset), buf, len);

	offset += IR_DATA_LENGTH;
	if(offset == IR_DATA_LENGTH*IR_DATA_NUM)
		offset = 0;
	
}

static int init_dsp_ir(void)
{
	int audio_fd = -1;
	audio_fd=open("/dev/silan-dsp-ir", O_RDWR);
	if(audio_fd<0){
		printf("fail to open /dev/silan-dsp-ir\n");
		return -1;
	}

	unsigned int tmp =  (unsigned long)mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, audio_fd, 0);

	dsp_ir_start_addr_va = (unsigned char *)tmp;

	memset((unsigned char *)dsp_ir_start_addr_va,0x00,0x1000);

	return 0;
}

#if 1
static SL_POINTER  get_ir(SL_POINTER Args)
{
	struct ifreq if0;
	int needRecv;
	unsigned char *buffer;
	unsigned short *tmp;
	// set socket's address information
	struct sockaddr_in   server_addr;

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(IR_SERVER_PORT);

	// create a stream socket
	//int server_socket = socket(PF_INET, SOCK_STREAM, 0); //TCP
	int server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);  //UDP
	if (server_socket < 0)
	{
		printf("Create Socket Failed!\n");
		exit(1);
	}

	if(interface == INTERFACE_WLAN0)
	{
		strncpy(if0.ifr_name,"wlan0",IFNAMSIZ);
		if(ioctl(server_socket,SIOCGIFHWADDR,&if0)<0)
		{
			printf("ioctl SIOCGIFHWADDR error\n");
			return -1;
		}
	}

	//bind
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Port: %d Failed!\n", IR_SERVER_PORT);
		exit(1);
	}
ReListen:
#if 0 //0:UDP 1:TCP
	// listen
	if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
	{
		printf("Server Listen Failed!\n");
		exit(1);
	}

	printf("listen ok\n");
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	
	int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
	if (new_server_socket < 0)
	{
		printf("Server Accept Failed!\n");
		//break;
	}
#endif
	//needRecv=sizeof(Node);
	needRecv=72*3*2;
	buffer=(unsigned char*)malloc(needRecv);
	memset(buffer,0x00,needRecv);
	int pos=0;
	int len;
	while(1)
	{
		pos=0;
		while(pos < needRecv)
		{
			//len = recv(new_server_socket, buffer + pos, needRecv-pos, 0);// TCP block receive 
			len = recv(server_socket, buffer + pos, needRecv - pos, 0);  //UDP
			if (len < 0)
			{
				printf("Server Recieve Data Failed!\n");
				break;
			}

			pos+=len;

		}
		if(needRecv != pos)
			printf("pos=%d \n",pos);
		printf("send ir \n");
#ifdef IR_DEBUG
		tmp = (unsigned short *)buffer;
		int i;
		//for (i=0;i<72;i++)
		for (i=0;i<215;i++)
		{
			printf("%d ",tmp[i]);
		}
		printf("\n");
#endif
		send_to_dsp(buffer, pos);
		printf("send to dsp \n");
		usleep(20000); //FIXME
	}
	//close(new_server_socket);
	free(buffer);
	close(server_socket);

	return 0;
}

#else
static SL_POINTER  get_ir(SL_POINTER Args)
{
	struct ifreq if0;
	int needRecv;
	unsigned char *buffer;
	unsigned char *tmp;
	// set socket's address information
	struct sockaddr_in   server_addr;

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(IR_SERVER_PORT);

	// create a stream socket
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		printf("Create Socket Failed!\n");
		exit(1);
	}

	if(interface == INTERFACE_WLAN0)
	{
		strncpy(if0.ifr_name,"wlan0",IFNAMSIZ);
		if(ioctl(server_socket,SIOCGIFHWADDR,&if0)<0)
		{
			printf("ioctl SIOCGIFHWADDR error\n");
			return -1;
		}
	}

	//bind
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Port: %d Failed!\n", IR_SERVER_PORT);
		exit(1);
	}
#if 0 //
	// listen
	if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
	{
		printf("Server Listen Failed!\n");
		exit(1);
	}

	printf("listen ok\n");
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
	if (new_server_socket < 0)
	{
		printf("Server Accept Failed!\n");
		//break;
	}
#endif

	needRecv=sizeof(Node);
	buffer=(unsigned char*)malloc(needRecv);
	memset(buffer,0x00,needRecv);
	int pos=0;
	int len;
	while(1)
	{
		pos=0;
		while(pos < needRecv)
		{
			len = recv(new_server_socket, buffer + pos, needRecv-pos, 0);//block receive 
			if (len < 0)
			{
				printf("Server Recieve Data Failed!\n");
				break;
			}

			pos+=len;

		}
		if(needRecv != pos)
			printf("pos=%d \n",pos);

#ifdef IR_DEBUG
		tmp = (unsigned short *)buffer;
		int i;
		//for (i=0;i<needRecv/2;i++)
		for (i=0;i<6;i++)
		{
			printf("%d ",tmp[i]);
		}
		printf("\n");
#endif
		send_to_dsp(buffer, pos);
		usleep(20000); //FIXME
	}
	close(new_server_socket);
	free(buffer);
	close(server_socket);

	return 0;
}

#endif
#endif

#ifdef WEB_ENABLE
static SL_POINTER  config_handle(SL_POINTER Args)//             for zhou 1.29
{
	while(1)
	{	
		//printf("ucUpdateFlag=%d\n",share_mem->ucUpdateFlag);
		if(1==share_mem->ucUpdateFlag)
		{
			AppWriteCfgInfotoFile();
			SLVENC_setBitrate(share_mem->sm_encoder_setting.usEncRate);
			share_mem->ucUpdateFlag = 0;
		}
		sleep(1);
	}
	return NULL;
}
#endif

static int tx_switch_multicast_main(void)
{
	printf("tx_switch_multicast_main  start \n");
	tx_switch_multicast();
	
	return 0;
}

int main(int argc, char* argv[])
{
	SL_S32 ret = -1;
	
#if 0
	char configs[256];
	ret = InitCfgInfo(&fd_config);
	if(!ret) {

		sprintf(configs, "%s%s", "ifconfig eth0 ",cfginfo.ip);
		system(configs);
		//update ip
		//sprintf(cfginfo.ip,  "%s", "192.168.1.10");
		//update_cfg_info(fd_config, &cfginfo);

	}
	close(fd_config);
#endif
	printf("********************system starting***************************\n");
	printf(PRINT_VERSION);
#ifdef WEB_ENABLE

	InitShareMem();
	
	AppInitCfgInfoDefault();
    printf("cfg init ok \n");
    
    ret = AppInitCfgInfoFromFile(&fd_config);

    printf("ret = %d\n",ret);
    if(ret<0)
    {
        if (NULL!=fd_config)
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
    //if(strcmp("192.168.1.3",share_mem->sm_eth_setting.strEthIp)!=0)
		init_eth();//   zhou
		//sleep(1);

#endif

	SLOS_CreateMutex(&mutexlock);
#ifdef DEBUG_OFF
	ret = pthread_create(&watchdogHandle, NULL, watchdog_handle, NULL);
	if (ret) {
		log_err("Failed to Create watchdogHandle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif
	register_pull_method(PullFromMdev,PullFromDsp, PushToMdev);

#ifdef H264_OUTPUT
	ret = h264_output();
#endif

#ifdef KVM_UART 
	ret = pthread_create(&app_tx_uart_handler, NULL, app_tx_uart_main, NULL);
	if (ret) {
		log_err("Failed to Create app_tx_uart_handler Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

/******** jason add 20180830 *****************/
#ifdef SWIT_MULTICAST
    printf("tx_witch_multicast_main test start : [main]\n");   
	ret = pthread_create(&switch_multicast_handler, NULL, tx_switch_multicast_main, NULL);
	if (ret) {
		log_err("Failed to Create tx_witch_multicast_main Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif
	//while (1) sleep(1);
#ifdef ENABLE_GET_IR
	init_dsp_ir();
	ret = pthread_create(&get_ir_handler, NULL, get_ir, NULL);
	if (ret) {
		log_err("Failed to Create get_ir Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

	//while (1) sleep(1);
#ifdef WEB_ENABLE
	ret = pthread_create(&ConfigHandle, NULL, sharemem_handle, NULL);
	if (ret) {
		log_err("Failed to Create Config Handle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#if 1
	ret = pthread_create(&chip_handler, NULL, sii9293_handler, NULL);
	if (ret) {
		log_err("Failed to Create chip hander Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#if 1
	ret = pthread_create(&pushMdev2List_handle, NULL, pushMdev2List, NULL);
	if (ret) {
		log_err("Failed to Create pushMdev2List_handle, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

	sleep(3);

#ifdef RINGBUFF
	ret = pthread_create(&audio_handler, NULL, audio_transfer, NULL);
	if (ret) {
		log_err("Failed to Create audio_transfer Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#ifdef VIU_OUTPUT
	ret = pthread_create(&viu_output_handle, NULL, viu_output, NULL);
	if (ret) {
		log_err("Failed to Create viu_output_handle, %d\n", ret);
		log_err("%d reboot",__LINE__);
		//reboot1();
		return ret;
	}
#endif

#ifdef APP_RTP
    printf("pull from list start \n");
	ret = pthread_create(&PullFromList_handler, NULL, PullFromList, NULL);
	if (ret) {
		log_err("Failed to Create rtsp Thread, %d\n", ret);
		return ret;
	}
#endif



#if 0//def RTSP_ENABLE

	char static_ip[20] = "10.10.1.1";
	memset((void *)&server_param, 0x00,  sizeof(server_param_t));
	
#if 1
	interface = INTERFACE_ETH0;
#else
	interface = INTERFACE_WLAN0;
#endif
	if(interface == INTERFACE_WLAN0)
	{
		sleep(2);//wait 8192du detected completely
		wifi_ap_start_hostapd();
		sleep(1);
		wifi_ap_set_static_ip(static_ip);
		//sleep(1);
		wifi_ap_start_udhcpd();
		sleep(1);
	}

	server_param.interface = interface;

	server_param.multicast = 0;//0; //1:UDP 0:TCP
#if 1
	char const *destinationAddressStr = "239.255.42.44";
	char const *destinationAddressStrAudio = "239.255.42.45";
#else
	char const *destinationAddressStr = "239.250.42.44";
	char const *destinationAddressStrAudio = "239.250.42.45";
#endif

	if(server_param.multicast)
	{
		strcpy(server_param.ipMulticastAddr, destinationAddressStr);	
		strcpy(server_param.ipMulticastAddrAudio, destinationAddressStrAudio);	
	}

#if 1
	ret = pthread_create(&rtspserver, NULL, StartRtsp, (void *)&server_param);
	if (ret) {
		log_err("Failed to Create rtsp Thread, %d\n", ret);
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#endif

#if 0
	ret = pthread_create(&setBitrateHandle, NULL, setBitrate_handle, NULL);
	if (ret) {
		log_err("Failed to Create setBitrateHandle Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif

#ifdef APP_CODE

	int signal_ch_a = 8000;
    int data_port_a = 8001;
#if 1
	ret = pthread_create(&app_tx_handler, NULL, app_tx_main, NULL);
	if (ret) {
		log_err("Failed to Create app_tx_main Thread\n");
		return ret;
	}
#endif
#ifdef APP_IO
	ret = pthread_create(&app_tx_light_ctl_handler, NULL, app_tx_light_ctl_main, NULL);
	if (ret) {
		log_err("Failed to Create data ch thread Thread\n");
		return ret;
	}
#endif
#if 1
	printf("app_tx_signal_ch_handler_a \n");
	ret = pthread_create(&app_tx_signal_ch_handler_a, NULL, app_tx_signal_ch_main, &signal_ch_a);
	if (ret) {
		log_err("Failed to Create signal ch Thread\n");
		return ret;
	}
#endif
#if 1
	ret = pthread_create(&app_tx_data_ch_handler_a, NULL, app_tx_data_ch_main, &data_port_a);
	if (ret) {
		log_err("Failed to Create data ch thread Thread\n");
		return ret;
	}
#endif
#endif
	//while(1) sleep(1);
#ifdef DEBUG_OFF
#if 1
	//signal(SIGINT, signalHandle);
	//signal(SIGTERM, signalHandle);
	//signal(SIGIO, signalioHandle);
#endif
#endif

	trigger();

	return 0;
}
