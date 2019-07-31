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
#include <pthread.h>
#include <linux/soundcard.h>
#include <errno.h>

#include "sl_watchdog.h"
#include "list_handler.h"
#include "cfginfo.h"
#include "ring_buffer.h"
#include <sys/time.h>

#include "uart_watchdog.h"
#include "../version.h"
#include "app_tx_io_ctl.h"
#include "sharemem.h"
#include "app_broadcast.h"

static pthread_t uartWatchdogHandler;
static pthread_t watchdogHandler;
static pthread_t controlHandler;
static SL_U32 need_feed_dog = 1;

void *mutexlock;

extern int reboot1(void);
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
			}
		}
	}

	return NULL;
}


int main(int argc, char* argv[])
{
	SL_S32 ret = -1;
	
	printf("********************control box system starting***************************\n");
	printf(PRINT_VERSION);
	System_running();
#if 0
	//watch dog thread	
	ret = pthread_create(&watchdogHandler, NULL, watchdog_handle, NULL);
	if (ret) {
		log_err("Failed to Create watchdogHandler Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}

	//uart watch dog thread
	ret = pthread_create(&uartWatchdogHandler, NULL, uart_watchdog, NULL);
	if (ret) {
		log_err("Failed to Create uartWatchdogHandler Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}
#endif
	//share memory thread
	InitShareMem();
	AppWriteCfgInfotoFile();

	//API thread


	//broadcast thread
	ret = pthread_create(&controlHandler, NULL, control_respond, NULL);
	if (ret) {
		log_err("Failed to Create uartWatchdogHandler Thread\n");
		log_err("%d reboot",__LINE__);
		reboot1();
		return ret;
	}	
	while(1) sleep(1);
	
	//

	
	return 0;
}
