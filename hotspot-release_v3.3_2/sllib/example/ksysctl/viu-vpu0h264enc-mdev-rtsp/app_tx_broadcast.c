
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
//#include "init.h"


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

void *control_slave()
{
	int sockfd = -1;
	int ret = -1;
	int len = -1;
	struct sockaddr_in servaddr;
	
	const int opt = -1;
	
	REPORT_PACK_S broadCollect_s;
	REPORT_PACK_S broadSend_s;
	
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
	
	while (1)
	{
		//sleep(1);
		if (readable_timeo(sockfd, 2))
		{
			len = recvfrom(sockfd, &broadCollect_s, sizeof(broadCollect_s), \
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
				printf("Ip : %d \n", broadCollect_s.ucIpAddress);
				printf("Multicast : %d \n", broadCollect_s.ucMultiAddress);
				//printf("uProbe : 0x%x \n", broadCollect_s.uProbe);
				//printf("uuid : %d \n", broadCollect_s.uuid);
				//printf("uuid : %d \n", broadCollect_s.uuid);
				if (PROBE == broadCollect_s.uProbe) //probe
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
					if (broadCollect_s.ucIpAddress <= 128) //ip
					{
						broadSend_s.ucIpAddress = broadCollect_s.ucIpAddress;
						broadSend_s.ucMultiAddress = broadCollect_s.ucMultiAddress;
						broadSend_s.uProbe = broadCollect_s.uProbe;
						broadSend_s.uuid = broadCollect_s.uuid;
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
		}
	}
	return 0;
}

