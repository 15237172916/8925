
#include <sys/types.h>
#include <sys/socket.h> 
#include <stdio.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <netinet/in.h>                         // 包含AF_INET相关结构
#include <arpa/inet.h>                      // 包含AF_INET相关操作的函数
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#include "app_broadcast.h"
#include "sharemem.h"
//#include "init.h"

static REPORT_PACK_S broadRecv_s;
static REPORT_PACK_S broadSend_s;

#if 1
static int readable_timeo(int fd, int sec)
{
	fd_set rset;
	struct timeval tv;
	
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	
	return (select(fd+1, &rset, NULL, NULL, &tv));
}
#endif

static void AVupdate(unsigned char order)
{	
	printf("AV update \n");
}

static void AVclean(unsigned char order)
{
	printf("AV clean \n");
}

void updateOffLineDeviceNumbers(int sig)
{
	unsigned char i = 0;

	if (SIGALRM == sig)
	{
		printf("update off-line device's numbers \n");
		//Check off-line RX's devices
		for (i=0; i<128; i++)
		{
			if (share_mem->rx_info[i].heart_count==0)
			{
				printf("RX[%d] is off-line \n", i);
			}
			else
			{
				share_mem->rx_info[i].heart_count = 0;
			}	
		}
		//Check off-line TX's devices 
		for (i=0; i<24; i++)
		{
			if (share_mem->tx_info[i].heart_count == 0)
			{
				printf("TX[%d] is off-line \n", i);	
			}
			else
			{
				share_mem->tx_info[i].heart_count = 0;
			}
		}
		alarm(20);
	}
	return ;
}
void *control_respond()
{
	int sockfd = -1;
	int ret = -1;
	int len = -1;
	int refresh_count = 0;
	unsigned char TX_ID = 0, RX_ID = 0, order = 0;
	struct sockaddr_in servaddr;
	
	const int opt = -1;
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_UDP_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("0.0.0.0"); 
	//servaddr.sin_addr.s_addr=htonl(INADDR_BROADCAST); //inet_addr("255.255.255.255"); 
	
	int servlen_addr_length = sizeof(servaddr);
try_socket:
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)	//socket UDP
	{
		perror("socket");
		printf("server broadcast socket error \n");
		
		goto try_socket;
	}
	
	printf("sockfd = %d \n", sockfd);
	
	//set socket broadcast
	ret = setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt));
	
	if (ret = bind(sockfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr)) < 0) //bind
	{
		perror("bind");
		printf("server broadcast bind error \n");
		close(sockfd);
		goto try_socket;
	}
	
	printf("bind secceed \n");
	
	struct timeval timeout={2,0};
	setsockopt(sockfd, SOL_SOCKET,SO_SNDTIMEO, &timeout, sizeof(timeout));
	signal(SIGALRM, updateOffLineDeviceNumbers);
	alarm(20);
	while (1)
	{
		//refresh_count++;
		//printf("clock : %ld \n", clock());

Recv:
		//sleep(1);
		if (readable_timeo(sockfd, 2))
		{
			len = recvfrom(sockfd, &broadRecv_s, sizeof(broadRecv_s), \
				0, (struct sockaddr_in *) &servaddr, &servlen_addr_length);
			if (len <= 0)
			{
				perror("recvfrom");
				printf("recv len = %d \n", len);
				printf("server_broadcast recvfrom error \n");
			}
			else
			{
				printf("ucIpAddress: %d \n", broadRecv_s.ucIpAddress);
				printf("ucRepayType:%d \n", broadRecv_s.ucRepayType);
				printf("ucCurrentState:%d \n", broadRecv_s.ucCurrentState);
				printf("ucProbe:%d \n", broadRecv_s.ucProbe);
				printf("uuid:%d \n", broadRecv_s.uuid);
				printf("audio_ch:%d \n", broadRecv_s.tx_info_s.audio_ch);
				printf("audio_sample:%d \n", broadRecv_s.tx_info_s.audio_sample);
				printf("is_hdmi_input:%d \n", broadRecv_s.tx_info_s.is_hdmi_input);
				printf("video_framrate:%d \n", broadRecv_s.tx_info_s.video_framrate);
				printf("video_height:%d \n", broadRecv_s.tx_info_s.video_height);
				printf("video_width:%d \n", broadRecv_s.tx_info_s.video_width);
				//printf("", broadRecv_s.tx_info_s);
				if (PROBE != broadRecv_s.ucProbe)
				{
					printf("PROBE is error \n");
					goto Recv;
				}
				broadSend_s.ucProbe = broadRecv_s.ucProbe;
				switch (broadRecv_s.ucRepayType)
				{
					case TX:
						printf("repay type is TX \n");
						broadSend_s.ucRepayType = TX;
						//printf("broadSend_s.ucRepayType : %d \n", broadSend_s.ucRepayType);
						TX_ID = broadRecv_s.ucIpAddress;
						order = TX_ID-200;
						if ((TX_ID < 200) || (TX_ID > 224))
						{
							printf("TX's ip address is error \n");
							continue;
						}
						broadSend_s.ucIpAddress = TX_ID;
						switch (broadRecv_s.ucCurrentState)
						{
							case HEART:
								share_mem->tx_info[order].heart_count++;
								printf("heart count: %d \n", share_mem->tx_info[order].heart_count);
								broadSend_s.ucCurrentState = HEART;
								continue;
								break;
							case START:
								printf("current state is START \n");
								if (broadRecv_s.uuid != share_mem->tx_info[order].uuid)
								{
									share_mem->tx_info[order].uuid = broadRecv_s.uuid;
								}
								broadSend_s.uuid = broadRecv_s.uuid;
								if (OFF == broadRecv_s.tx_info_s.is_hdmi_input)
								{
									AVclean(order);
									broadSend_s.ucCurrentState = START;
									printf("HDMI not input \n");
									goto Send;
								}
								AVupdate(order);
								broadSend_s.ucCurrentState = RESPOND;
								break;
							case RESPOND:
								broadSend_s.ucCurrentState = HEART;
								
								break;
						}
						break;
					case RX:
						printf("repay type is RX \n");
						RX_ID = broadRecv_s.ucIpAddress;
						order = RX_ID;
						if ((RX_ID < 1) || (RX_ID > 128))
						{
							printf("RX's ip address is error \n");
							continue;
						}
						break;
					default:
						printf("repay type is error \n");
						continue;
						
				}
				
			}
Send:
			len = sendto(sockfd, &broadSend_s, sizeof(broadSend_s), \
					0, (struct sockaddr *)&servaddr, sizeof(servaddr));
			if (len <= 0)
			{
				perror("sendto");
				printf("send len = %d \n", len);
				printf("server_broadcast sendto error \n");
			}
			else
			{

			}

			#if 0
			len = recvfrom(sockfd, &broadRecv_s, sizeof(broadRecv_s), \
				0, (struct sockaddr_in *) &servaddr, &servlen_addr_length);
			if (len <= 0)
			{
				perror("recvfrom");
				printf("recv len = %d \n", len);
				printf("server_broadcast recvfrom error \n");
			}
			else
			{
				usleep(1000);
				//printf("recv len = %d \n", len);
				//printf("Ip : %d \n", broadRecv_s.ucIpAddress);
				//printf("Multicast : %d \n", broadRecv_s.ucMultiAddress);
				//printf("uProbe : 0x%x \n", broadRecv_s.uProbe);
				//printf("uuid : %d \n", broadRecv_s.uuid);
				//printf("uuid : %d \n", broadRecv_s.uuid);
				if (PROBE == broadRecv_s.uProbe) //probe
				{
					//printf("share_mem->ucInfoDisplayFlag: %d\n",share_mem->ucInfoDisplayFlag);
					if (1 == share_mem->ucInfoDisplayFlag)
					{
						broadSend_s.ucInfoDisplayFlag = 1;
						goto INFO;
					}
					else
					{
						broadSend_s.ucInfoDisplayFlag = 0;
					}
					if (broadRecv_s.ucIpAddress <= 128) //ip
					{
						broadSend_s.ucIpAddress = broadRecv_s.ucIpAddress;
						broadSend_s.ucMultiAddress = broadRecv_s.ucMultiAddress;
						broadSend_s.uProbe = broadRecv_s.uProbe;
						broadSend_s.uuid = broadRecv_s.uuid;
					}
					else
					{
						printf("Slave ip address is error\n");
						continue;
					}
				}
				if (share_mem->sm_group_pack.uuid[broadSend_s.ucIpAddress] == broadSend_s.uuid) //uuid
				{
					if (share_mem->sm_group_pack.ucMultiAddress[broadSend_s.ucIpAddress] == broadSend_s.ucMultiAddress)
					{
						//printf("Multicast is the same \n");
						continue;
					}
					else
					{
						broadSend_s.ucMultiAddress = share_mem->sm_group_pack.ucMultiAddress[broadSend_s.ucIpAddress];
						//broadSend_s.uuid = share_mem->sm_group_pack.uuid[broadSend_s.ucIpAddress];
					}
				}
				else
				{
					share_mem->sm_group_pack.uuid[broadSend_s.ucIpAddress] = broadSend_s.uuid;
					//share_mem->sm_group_pack.ucMultiAddress[broadSend_s.ucIpAddress] = broadSend_s.ucMultiAddress;
					share_mem->ucUpdateFlag = 1;
				}
#if 1
INFO:
				len = sendto(sockfd, &broadSend_s, sizeof(broadSend_s), \
					0, (struct sockaddr *)&servaddr, sizeof(servaddr));
				if (len <= 0)
				{
					perror("sendto");
					printf("send len = %d \n", len);
					printf("server_broadcast sendto error \n");
				}
				else
				{
					//printf("sendto success \n");
				}
#endif
			}
			#endif
		}
	}
	return 0;
}

