#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/socket.h>                         // 包含套接字函数库
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>                         // 包含AF_INET相关结构
#include <math.h>
#include "../version.h"
#include "IR.h"
#include "sharemem.h"
#include "digit_led.h"

static unsigned char *dsp_ir_start_addr_va;
extern char multicast[20];

int init_dsp_ir(void)
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

#if 1
SL_POINTER  get_ir(SL_POINTER Args)
{
	int needRecv;
	int ret;
	unsigned char *buffer;
	unsigned short *tmp;
	// set socket's address information
	struct sockaddr_in   server_addr;
    
   //int server_socket, new_server_socket;
	int rtp_server_socket;
	//struct sockaddr_in server_addr,  client_addr;
	
ReSocket:
	printf("starts creat socket \n");
	//web_flag = 0;

	//struct tcp_info info;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(7777);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	//create a stream socket
	rtp_server_socket = socket(AF_INET, SOCK_DGRAM, 0); //UDP
	if (rtp_server_socket < 0)
	{
		printf("Create Socket Failed!\n");
		perror("socket");
		sleep(1);
		goto ReSocket;
	}
	
    printf("Create Socket OK \n");
    
    //bind
    if (bind(rtp_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Bind Port: 7777 Failed!\n");
        perror("bind");
        close(rtp_server_socket);
        sleep(1);
        goto ReSocket;
    }
    printf("bind socket ok \n");

	printf ("%s started.\n", __func__);
	printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );

	//set Multicast loop
	int loop = 1; //1: on , 0: off
	ret = setsockopt(rtp_server_socket,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop));
	if(ret < 0)
	{
		printf("set multicast loop error \n");
		perror("setsockopt");
	}

	//set multicast address 
	struct ip_mreq mreq;
	//mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);       
	mreq.imr_multiaddr.s_addr = inet_addr(multicast);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);     
	
	//add multicast group
	ret = setsockopt(rtp_server_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if(ret < 0)
	{
		printf("set multicast error \n");
		perror("setsockopt");
	}
	
	int reuse = 1;
	ret = setsockopt(rtp_server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
	if(ret < 0)
	{
		printf("set multicast reuse error \n");
		perror("setsockopt");
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
			len = recv(rtp_server_socket, buffer + pos, needRecv - pos, 0);  //UDP
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
	close(rtp_server_socket);

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