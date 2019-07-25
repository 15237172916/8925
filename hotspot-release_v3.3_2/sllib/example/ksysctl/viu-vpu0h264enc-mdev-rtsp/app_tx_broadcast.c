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

char display_flag = 0;

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

void *IP_broadcast_report()
{
	int sockfd = -1;
	int ret = -1;
	int len = -1;
	
	
	const int opt = -1;
	unsigned int ip_add, mul_add;
	int random_number = get_random();
	//int random_number = share_mem->uuid;
	socklen_t servlen_addr_length;
	struct sockaddr_in servaddr;
	char * s = malloc(20*sizeof(char));
	REPORT_PACK_S broadReport_s;
	REPORT_PACK_S broadRecv_s;
	
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
	broadReport_s.ucProbe = PROBE;
	broadReport_s.uuid = random_number; //random number
	broadReport_s.ucCurrentState = START; 
	broadReport_s.ucRepayType = TX;

	while (1)
	{
		sleep(1);
		
		//get multicast address
		inet_pton(AF_INET, share_mem->sm_eth_setting.strEthMulticast, &mul_add);
		mul_add = ntohl(mul_add); //host
		mul_add &= 0xFF;
		broadReport_s.rx_info_s.video_source = mul_add; //multicast address
		printf("multicast address : %d \n", mul_add);
		
		//get ip address
		inet_pton(AF_INET, share_mem->sm_eth_setting.strEthIp, &ip_add);
		ip_add = ntohl(ip_add); //host
		ip_add &= 0xFF;
		broadReport_s.ucIpAddress = ip_add-1; //ip address, ip address is 1-128 but ucIpAddress is 0-127.
		printf("ip address : %d \n", ip_add);
		
		//printf("broadReport_s.ucIpAddress : %d \n", broadReport_s.ucIpAddress);
		//printf("broadReport_s.ucMultiAddress : %d \n", broadReport_s.ucMultiAddress);
		//printf("buffer 1 : %d \n", buffer_flag[1]);
		
		len = sendto(sockfd, &broadReport_s, sizeof(broadReport_s), \
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
					//printf("ucInfoDisplayFlag : %d \n", broadRecv_s.ucInfoDisplayFlag);
					//printf("ucIpAddress : %d \n", broadRecv_s.ucIpAddress);
					//printf("ucMultiAddress : %d \n", broadRecv_s.ucMultiAddress);
					//printf("uProbe : 0x%x \n", broadRecv_s.uProbe);
					//printf("uuid : %d \n", broadRecv_s.uuid);
					#if 0
					if (PROBE != broadRecv_s.uProbe)
					{
						printf("probe error \n");
						continue;
					}
					else
					{
						display_flag = broadRecv_s.ucInfoDisplayFlag;
						//printf("ucInfoDisplayFlag: %d \n", broadRecv_s.ucInfoDisplayFlag);
					}
					if (broadReport_s.ucIpAddress != broadRecv_s.ucIpAddress)
					{
						printf("IP address is not same \n");
						continue;
					}
					else if (broadReport_s.uuid != broadRecv_s.uuid)
					{
						printf("uuid is error \n");
						continue;
					}
					else if (broadReport_s.ucMultiAddress == broadRecv_s.ucMultiAddress)
					{
						printf("Multicast address is same \n");
						sprintf(s, "239.255.42.%d", broadRecv_s.ucMultiAddress);
						printf(s);
						continue;
					}
					else //change multicast address
					{
						sprintf(s, "239.255.42.%d", broadRecv_s.ucMultiAddress);
						printf(s);
						strcpy(share_mem->sm_eth_setting.strEthMulticast, s);
						share_mem->ucUpdateFlag = 1;
					}
					#endif
				}
			}
			else
			{
				display_flag = 0;
			}
		}
	}
	
	close(sockfd);
	
	return 0;
}
