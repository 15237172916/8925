#ifndef __MAIN_H__
#define __MAIN_H_
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
#include <linux/soundcard.h>
#include <errno.h>
#include <sys/time.h>


#define CP_CONFIG_00 "/bin/cp /tmp/configs/config.conf /tmp/configs/config0.conf"
#define CP_CONFIG_01 "/bin/cp /tmp/configs/config.conf /tmp/configs/config1.conf"
#define CP_CONFIG_02 "/bin/cp /tmp/configs/config.conf /tmp/configs/config2.conf"
#define CP_CONFIG_03 "/bin/cp /tmp/configs/config.conf /tmp/configs/config3.conf"
#define CP_CONFIG_04 "/bin/cp /tmp/configs/config.conf /tmp/configs/config4.conf"
#define CP_CONFIG_05 "/bin/cp /tmp/configs/config.conf /tmp/configs/config5.conf"
#define CP_CONFIG_06 "/bin/cp /tmp/configs/config.conf /tmp/configs/config6.conf"
#define CP_CONFIG_07 "/bin/cp /tmp/configs/config.conf /tmp/configs/config7.conf"
#define CP_CONFIG_08 "/bin/cp /tmp/configs/config.conf /tmp/configs/config8.conf"
#define CP_CONFIG_09 "/bin/cp /tmp/configs/config.conf /tmp/configs/config9.conf"
#define CP_CONFIG_10 "/bin/cp /tmp/configs/config.conf /tmp/configs/config10.conf"

#define BACKUP_00 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config0.conf /tmp/configs/config.conf"
#define BACKUP_01 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config1.conf /tmp/configs/config.conf"
#define BACKUP_02 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config2.conf /tmp/configs/config.conf"
#define BACKUP_03 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config3.conf /tmp/configs/config.conf"
#define BACKUP_04 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config4.conf /tmp/configs/config.conf"
#define BACKUP_05 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config5.conf /tmp/configs/config.conf"
#define BACKUP_06 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config6.conf /tmp/configs/config.conf"
#define BACKUP_07 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config7.conf /tmp/configs/config.conf"
#define BACKUP_08 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config8.conf /tmp/configs/config.conf"
#define BACKUP_09 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config9.conf /tmp/configs/config.conf"
#define BACKUP_10 "cd /tmp/&&./UpGradeServer 224 /tmp/configs/config10.conf /tmp/configs/config.conf"



#define WIDTH 1920
#define HEIGHT 1080

#define USLEEP_TIME	    5*1000
#define LIVE_TIME_SECONDS 20

#define DEV_IO_NAME		"/dev/silan_testio"

#endif