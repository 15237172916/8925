#ifndef __MAIN_H_
#define __MAIN_H_

#include <sys/types.h>
#include <sys/socket.h>						 // 包含套接字函数库
#include <stdio.h>
#include <netinet/in.h>						 // 包含AF_INET相关结构
#include <arpa/inet.h>					  // 包含AF_INET相关操作的函数
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
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


/*******************************define*******************************/

#define NUM_RETRY	   50000
#define USLEEP_TIME	 5*1000
#define TIMEOUT   -1

#ifdef HANG_PAI
#define MAX_RTSP_TIMEOUT  3
#else
#define MAX_RTSP_TIMEOUT  8
#endif

#define TIME_CHECK_SECONDS 2





#endif