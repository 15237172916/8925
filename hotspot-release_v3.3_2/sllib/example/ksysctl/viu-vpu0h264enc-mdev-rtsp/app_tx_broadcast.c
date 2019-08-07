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

#include "app_tx_broadcast.h"
#include "sharemem.h"
#include "init.h"

//char display_flag = 0;
static REPORT_PACK_S broadSend_s;
static REPORT_PACK_S broadRecv_s;

#if 1
static int get_random(void)
{
	struct timeval tv;
    gettimeofday(&tv,NULL);
	
	//printf("(unsigned int)time(NULL) : %d \n", (unsigned int)time(NULL));
	srand((unsigned int)(tv.tv_sec*1000000 + tv.tv_usec));
	
	return rand(); 
}
#endif

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

static void AVclean(void)
{
	printf("AV clean \n");
}

static void AVupdate(void)
{
	broadSend_s.tx_info_s.is_hdmi_input = share_mem->sm_tx_info.is_hdmi_input;
	broadSend_s.tx_info_s.audio_ch = share_mem->sm_tx_info.audio_ch;
	broadSend_s.tx_info_s.audio_sample = share_mem->sm_tx_info.audio_sample;
	broadSend_s.tx_info_s.audio_type = share_mem->sm_tx_info.audio_type;
	broadSend_s.tx_info_s.video_framrate = share_mem->sm_tx_info.video_framrate;
	broadSend_s.tx_info_s.video_height = share_mem->sm_tx_info.video_height;
	broadSend_s.tx_info_s.video_width = share_mem->sm_tx_info.video_width;
	strcpy(broadSend_s.tx_info_s.fw_version, share_mem->sm_tx_info.fw_version);

	printf("AV update");
}

void *IP_broadcast_ask()
{
	int sockfd = -1;
	int ret = -1;
	int len = -1;
	
	const int opt = -1;
	unsigned int ip_add, mul_add;
	//int random_number = get_random();
	int random_number = share_mem->uuid;
	socklen_t servlen_addr_length;
	struct sockaddr_in servaddr;
	char * s = malloc(20*sizeof(char));

	servlen_addr_length = sizeof(servaddr);
	memset(&servaddr, 0, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(CLI_UDP_PORT);
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST); //broadcast address
	//servaddr.sin_addr.s_addr = inet_addr("192.168.1.6");
	//servaddr.sin_addr.s_addr = inet_addr("255.255.255.0");
	//servaddr.sin_addr.s_addr = inet_addr("192.168.1.255");
	
try_socket:

	printf("start socked \n");
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)	//socket UDP
	{
		printf("client broadcast socket error \n");
		goto try_socket;
	}
	else
		printf("creat socket success \n");
		
	//set socket broadcast 
	ret = setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt)); 
	broadSend_s.uProbe = PROBE;
	broadSend_s.uuid = random_number; //random number
	broadSend_s.ucSignal = START; 
	broadSend_s.ucRepayType = TX;
	printf("REPORT_PACK_S : %d \n", sizeof(REPORT_PACK_S));
	AVupdate();
	while (1)
	{
		sleep(1);
		
		//get multicast address
		inet_pton(AF_INET, share_mem->sm_eth_setting.strEthMulticast, &mul_add);
		mul_add = ntohl(mul_add); //host
		mul_add &= 0xFF;
		broadSend_s.rx_info_s.video_source = mul_add; //multicast address
		//printf("multicast address : %d \n", mul_add);
		
		//get ip address
		inet_pton(AF_INET, share_mem->sm_eth_setting.strEthIp, &ip_add);
		ip_add = ntohl(ip_add); //host
		ip_add &= 0xFF;
		broadSend_s.ucIpAddress = ip_add; //ip address, ip address is 200-224
		//printf("ip address : %d \n", ip_add);
		
		//printf("broadSend_s.ucIpAddress : %d \n", broadSend_s.ucIpAddress);
		//printf("broadSend_s.ucMultiAddress : %d \n", broadSend_s.ucMultiAddress);
		//printf("buffer 1 : %d \n", buffer_flag[1]);
			
		broadSend_s.tx_info_s.is_hdmi_input = share_mem->sm_tx_info.is_hdmi_input;
		//printf("broadSend_s.tx_info_s.is_hdmi_input: %d \n", broadSend_s.tx_info_s.is_hdmi_input);
		len = sendto(sockfd, &broadSend_s, sizeof(broadSend_s), \
				0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		if (len <= 0)
		{
			perror("sendto");
			printf("send len = %d \n", len);
			printf("client broadcast sendto error \n");
			close(sockfd);
			goto try_socket;
		}
		else
		{
			printf("TX sendto succeed, len: %d \n", len);
			if (readable_timeo(sockfd, 2))
			{
				len = recvfrom(sockfd, &broadRecv_s, sizeof(broadRecv_s), \
				0, (struct sockaddr_in *) &servaddr, &servlen_addr_length);
				if (len <= 0)
				{
					perror("recvfrom");
					printf("recv len = %d \n", len);
					printf("client broadcast recvfrom error \n");
				}
				else
				{
					printf("broadRecv_s.ucRepayType: %d \n", broadRecv_s.ucRepayType);
					printf("ucIpAddress : %d \n", broadRecv_s.ucIpAddress);
					printf("uProbe : 0x%x \n", broadRecv_s.uProbe);
					printf("uuid : %d \n", broadRecv_s.uuid);

					if (PROBE != broadRecv_s.uProbe)
					{
						printf("PROBE is error \n");
						continue;
					}
					if (broadRecv_s.uuid != broadSend_s.uuid)
					{
						printf("uuid is error \n");
						continue;
					}
					if (broadRecv_s.ucRepayType != TX)
					{
						printf("repay type is error \n");
						printf("broadRecv_s.ucRepayType: %d \n", broadRecv_s.ucRepayType);
						continue;
					}
					if (broadSend_s.ucIpAddress != broadRecv_s.ucIpAddress)
					{
						printf("ip address is error \n");
						continue;
					}
					switch (broadRecv_s.ucSignal)
					{
						case HEART:
							//broadSend_s.ucSignal = HEART;
							//AVclean();
							if (1 == share_mem->sm_tx_info.is_hdmi_input)
							{
								AVupdate();
							}
							else
							{
								AVclean();
							}
							
							break;
						case START:
							AVupdate();
							broadSend_s.ucSignal = START;
							break;
						case RESPOND:
							broadSend_s.ucSignal = HEART;
							AVclean();
							break;
					}
				}
			}
		}
	}
	
	close(sockfd);
	
	return 0;
}
