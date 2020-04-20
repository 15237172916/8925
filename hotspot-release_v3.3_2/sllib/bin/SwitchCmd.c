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


#define SERV_UDP_PORT	8889


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


int main(char argc, char *argv[])
{
	if (argc < 3)
	{
		printf("command error \n");
		printf("cmd rx tx \n");
		return 0;
	}

	unsigned char cmd[5];
	unsigned char recive_cmd[6];
	int sockfd = -1;
	int ret = -1;
	int len = -1;
	struct sockaddr_in servaddr;
	
	const int opt = -1;
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_UDP_PORT);
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("0.0.0.0"); 
	servaddr.sin_addr.s_addr=htonl(INADDR_BROADCAST); //inet_addr("255.255.255.255"); 
	
	int servlen_addr_length = sizeof(servaddr);
try_socket:
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)	//socket UDP
	{
		perror("socket");
		printf("server broadcast socket error \n");
		
		goto try_socket;
	}
	
	//printf("sockfd = %d \n", sockfd);
	
	//set socket broadcast
	ret = setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt));
	#if 0
	if (ret = bind(sockfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr)) < 0) //bind
	{
		perror("bind");
		printf("server broadcast bind error \n");
		close(sockfd);
		goto try_socket;
	}
	#endif
	//printf("bind secceed \n");
	
	struct timeval timeout={2,0};
	setsockopt(sockfd, SOL_SOCKET,SO_SNDTIMEO, &timeout, sizeof(timeout));

	int i;
	
	cmd[0] = 0xfe;
	cmd[1] = 0x01;
	cmd[2] = 2;//argc - 1; 
	//printf("argv[0] : %s \n", argv[0]);
	//printf("argv[1] : %s \n", argv[1]);
	//printf("argv[2] : %s \n", argv[2]);
	//printf("argv[3] : %s \n", argv[3]);
	//printf("argv[4] : %s \n", argv[4]);

	int rx = atoi(argv[1]);
	int tx = atoi(argv[2]);

	printf("rx : %d \n", rx);
	printf("tx : %d \n", tx);

#if 1
INFO:
		cmd[3] = rx;
		cmd[4] = tx;
		
		len = sendto(sockfd, &cmd, sizeof(cmd), \
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
			//sleep(1);
		}
#endif	
		if (readable_timeo(sockfd, 2))
		{
			len = recvfrom(sockfd, &recive_cmd, sizeof(recive_cmd), \
			0, (struct sockaddr_in *) &servaddr, &servlen_addr_length);
			if (len <= 0)
			{
				perror("recvfrom");
				printf("recv len = %d \n", len);
				printf("client broadcast recvfrom error \n");
			}
			else
			{
				printf("cmd[0] = %d\n", recive_cmd[0]);
				printf("cmd[1] = %d\n", recive_cmd[1]);
				printf("cmd[2] = %d\n", recive_cmd[2]);
				printf("cmd[3] = %d\n", recive_cmd[3]);
				printf("cmd[4] = %d\n", recive_cmd[4]);
				printf("cmd[5] = %d\n", recive_cmd[5]);
				if (recive_cmd[0] != 0xfe)
				{
					printf("0\n");
					printf("switch failed \n");
					return 0;
				}
				else if (recive_cmd[1] != 0xf1)
				{
					printf("1\n");
					printf("switch failed \n");
					return 0;
				}
				else if (recive_cmd[2] != 3)
				{
					printf("2\n");
					printf("switch failed \n");
					return 0;
				}
				else if (recive_cmd[3] != cmd[3])
				{
					printf("3\n");
					printf("switch failed \n");
					return 0;
				}
				else if (recive_cmd[4] != cmd[4])
				{
					printf("4\n");
					printf("switch failed \n");
					return 0;
				}
					
				else if (recive_cmd[5] != 1)
				{
					printf("5\n");
					printf("switch failed \n");
					return 0;
				}
				else
				{
					printf("switch succeed \n");
				}
					
			}
		}
		else
		{
			printf("switch failed \n");
		}
	
	return 0;
}




