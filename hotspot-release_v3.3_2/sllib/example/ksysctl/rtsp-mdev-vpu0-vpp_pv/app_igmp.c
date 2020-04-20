#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "app_igmp.h"

extern char multicast[20];
extern char web_flag;
char report_succeed = 1;

static unsigned short csum(unsigned short *buf, int nwords)
{
	unsigned long sum;
	
	//printf("buf : 0x%x \n", buf);
	
	for (sum = 0; nwords > 0; nwords--)
	{
		sum += *buf++;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	//printf("buf.usChecksum: 0x%x \n", ~sum);
	return (unsigned short)(~sum);
}

int IGMP_config(const char type)
{
	static int multicast_tmp = 0;
	
	if (IGMP_LEAVE==type)
	{
		IGMP_Packet.ucType = type; //report packet
		IGMP_Packet.ucMaxRspCode = 0; //tmieout
		IGMP_Packet.uiAddGroup = multicast_tmp; //leave address
		IGMP_Packet.usChecksum = 0;
		IGMP_Packet.usChecksum = csum(&IGMP_Packet, sizeof(IGMP_V2));
		//printf("usChecksum : 0x%x \n", IGMP_Packet.usChecksum);
	}
	else
	{
		IGMP_Packet.ucType = type; //report packet
		IGMP_Packet.ucMaxRspCode = 0; //tmieout
		IGMP_Packet.uiAddGroup = inet_addr(multicast); //report address
		IGMP_Packet.usChecksum = 0;
		IGMP_Packet.usChecksum = csum(&IGMP_Packet, sizeof(IGMP_V2));
		//printf("usChecksum : 0x%x \n", IGMP_Packet.usChecksum);
		multicast_tmp = inet_addr(multicast);
	}
}

//void main(void)
void *app_igmp_report()
{
	//printf("igmp : %d \n", sizeof(IGMP_V2));
	//printf("unsigned long int : %ld \n", sizeof(unsigned long int));
	int len, ret;
	
#if 1
	//set socket's address information
	int sock_cli;
	socklen_t servlen_addr_length;
    struct sockaddr_in server_addr;
    servlen_addr_length = sizeof(server_addr);
    memset(&server_addr, 0, sizeof(server_addr));
    
ReSocket:
    web_flag = 0;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(IGMP_PORT);
	//server_addr.sin_addr.s_addr=htonl(INADDR_BROADCAST); //UDP broadcast address 
	//server_addr.sin_addr.s_addr=inet_addr(MCAST_ADDR); //UDP multicast address
	server_addr.sin_addr.s_addr=inet_addr(multicast); //UDP multicast address
	
    //sock_cli = socket(AF_INET,SOCK_DGRAM, 0); //UDP
    sock_cli = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP); //IGMP
    if (sock_cli < 0)
    {
		perror("socket");
		sleep(1);
		goto ReSocket;
	}
#if 0
	const int opt = -1;
	setsockopt(sock_cli,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt));
#endif
	
#if 0 //client
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
#endif

#if 0
	int reuse = 1;
	ret = setsockopt(sock_cli, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
	if(ret < 0)
	{
		printf("set multicast reuse error \n");
		perror("setsockopt");
	}
#endif

#if 0
	const int on = 1;
	ret = setsockopt(sock_cli, IPPROTO_IP, IP_HDRINCL, (int *)&on, sizeof(on));
	if(ret < 0)
	{
		printf("set multicast ip error \n");
		perror("setsockopt");
	}
#endif

	struct timezone tz;
	struct timeval tv1, tv2;
	struct timeval timeout = {2,0};
	setsockopt(sock_cli,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
	setsockopt(sock_cli,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

#endif
	IGMP_config(IGMP_REPORT);
	//unsigned long int test = 0x17020802e00000fb;
	//unsigned long int test = 0xfb0000e002080217;
	while (1)
	{
Resend:	
		//IGMP_config(IGMP_REPORT);
		//printf("report succeed : %d \n", report_succeed);
		//printf("web_flag = %d \n", web_flag);
		if (1 == web_flag)
		{
			report_succeed = 0;
			server_addr.sin_addr.s_addr=inet_addr(LEAVE_ADDR);
			IGMP_config(IGMP_LEAVE);
			sendto(sock_cli, &IGMP_Packet, sizeof(IGMP_V2), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
			close(sock_cli);
			goto ReSocket;
		}
		
		len = sendto(sock_cli, &IGMP_Packet, sizeof(IGMP_V2), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		//len = sendto(sock_cli, &test, sizeof(test), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if (len < 0)
		{
			printf("len : %d \n", len);
			perror("sendto");
			sleep(1);
			goto Resend;
		}
		
		//printf("send igmp len : %d \n", len);
		usleep(500000); //0.5s
	}
}


