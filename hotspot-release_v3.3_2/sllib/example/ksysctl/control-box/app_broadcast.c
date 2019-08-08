
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
	share_mem->tx_info[order].audio_ch = broadRecv_s.tx_info_s.audio_ch;
	share_mem->tx_info[order].audio_sample = broadRecv_s.tx_info_s.audio_sample;
	share_mem->tx_info[order].audio_type = broadRecv_s.tx_info_s.audio_type;
	share_mem->tx_info[order].video_framrate = broadRecv_s.tx_info_s.video_framrate;
	share_mem->tx_info[order].video_height = broadRecv_s.tx_info_s.video_height;
	share_mem->tx_info[order].video_width = broadRecv_s.tx_info_s.video_width;
	share_mem->tx_info[order].is_hdmi_input = broadRecv_s.tx_info_s.is_hdmi_input;
	strcpy(share_mem->tx_info[order].fw_version, broadRecv_s.tx_info_s.fw_version);

	printf("AV update \n");
}

static void AVclean(unsigned char order)
{
	share_mem->tx_info[order].audio_ch = 0;
	share_mem->tx_info[order].audio_sample = 0;
	share_mem->tx_info[order].audio_type = 0;
	share_mem->tx_info[order].video_framrate = 0;
	share_mem->tx_info[order].video_height = 0;
	share_mem->tx_info[order].video_width = 0;
	share_mem->tx_info[order].is_hdmi_input = 0;

	printf("AV clean \n");
}

static void controlDataUpdate(unsigned char order)
{
	//update osd status
	broadSend_s.rx_info_s.osd_status = share_mem->rx_info[order].osd_status;
	//update tv status
	broadSend_s.rx_info_s.tv_status = share_mem->rx_info[order].tv_status;
	//printf("rx %d share video %d \n", order, share_mem->rx_info[order].video_source);
	broadSend_s.rx_info_s.data_type = share_mem->rx_info[order].data_type;
	//update video source
	broadSend_s.rx_info_s.video_source = share_mem->rx_info[order].video_source;
	//update control data from control box to RX
	broadSend_s.rx_info_s.control_data.baud_rate = share_mem->rx_info[order].control_data.baud_rate;
	broadSend_s.rx_info_s.control_data.data_bit = share_mem->rx_info[order].control_data.data_bit;
	broadSend_s.rx_info_s.control_data.data_format = share_mem->rx_info[order].control_data.data_format;
	broadSend_s.rx_info_s.control_data.parity_bit = share_mem->rx_info[order].control_data.parity_bit;
	strcpy(broadSend_s.rx_info_s.control_data.off_data, share_mem->rx_info[order].control_data.off_data);
	strcpy(broadSend_s.rx_info_s.control_data.on_data, share_mem->rx_info[order].control_data.on_data);
	//update software version from RX to control box
	strcpy(share_mem->rx_info[order].fw_version, broadRecv_s.rx_info_s.fw_version);
	//printf("broad send rx video source %d \n", broadSend_s.rx_info_s.video_source);
	printf("control data update \n");
}

static void controlDataClean(unsigned char order)
{

	printf("control data clean \n");
}

void updateOffLineDeviceNumbers(int sig)
{
	unsigned char i = 0;
	unsigned char tx_online_count = 0, rx_online_count = 0;

	if (SIGALRM == sig)
	{
		printf("update off-line device numbers \n");
		//Check off-line RX's devices
		for (i=0; i<128; i++)
		{
			if (share_mem->rx_info[i].heart_count==0)
			{
				//printf("RX[%d] is off-line \n", i+1);
			}
			else
			{
				rx_online_count++;
				share_mem->rx_info[i].heart_count = 0;
			}
			share_mem->rx_info[i].online_count = 0;
		}
		//Check off-line TX's devices 
		for (i=0; i<24; i++)
		{
			if (share_mem->tx_info[i].heart_count == 0)
			{
				//printf("TX[%d] is off-line \n", i+1);	
			}
			else
			{
				tx_online_count++;
				share_mem->tx_info[i].heart_count = 0;
			}
		}
		printf("tx on-line number is %d \n", tx_online_count);
		printf("rx on-line number is %d \n", rx_online_count);

		alarm(40);
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
	broadSend_s.ucSignal = START;	
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
	printf("REPORT_PACK_S : %d \n", sizeof(REPORT_PACK_S));
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
				//printf("ucRepayType:%d \n", broadRecv_s.ucRepayType);
				//printf("ucSignal:%d \n", broadRecv_s.ucSignal);
				//printf("ucProbe:%d \n", broadRecv_s.ucProbe);
				//printf("uuid:%d \n", broadRecv_s.uuid);
				
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
						printf("repay type is TX , receive len : %d \n", len);
						//printf("audio_ch:%d \n", broadRecv_s.tx_info_s.audio_ch);
						//printf("audio_sample:%d \n", broadRecv_s.tx_info_s.audio_sample);
						//printf("is_hdmi_input:%d \n", broadRecv_s.tx_info_s.is_hdmi_input);
						//printf("video_framrate:%d \n", broadRecv_s.tx_info_s.video_framrate);
						//printf("video_height:%d \n", broadRecv_s.tx_info_s.video_height);
						//printf("video_width:%d \n", broadRecv_s.tx_info_s.video_width);
						
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
						switch (broadRecv_s.ucSignal)
						{
							case HEART:
								share_mem->tx_info[order].heart_count++;
								//printf("heart count: %d \n", share_mem->tx_info[order].heart_count);
								broadSend_s.ucSignal = HEART;
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
									broadSend_s.ucSignal = START;
									printf("HDMI not input \n");
									goto Send;
								}
								AVupdate(order);
								broadSend_s.ucSignal = RESPOND;
								break;
							case RESPOND:
								broadSend_s.ucSignal = HEART;
								
								break;
						}
						break;
					case RX:
						//printf("rx recv video source %d \n", broadRecv_s.rx_info_s.video_source);
						printf("repay type is RX , receive len : %d\n", len);
						broadSend_s.ucRepayType = RX;
						//printf("rx send video source : %d \n", broadSend_s.rx_info_s.video_source);	
						RX_ID = broadRecv_s.ucIpAddress;
						order = RX_ID;
						if ((RX_ID < 1) || (RX_ID > 128))
						{
							printf("RX's ip address is error \n");
							continue;
						}
						broadSend_s.ucIpAddress = RX_ID;
						//judge rx's device repeat number
						if (broadRecv_s.uuid != share_mem->rx_info[order].uuid)
						{
							share_mem->rx_info[order].uuid = broadRecv_s.uuid;
							share_mem->rx_info[order].online_count++;
							printf("RX[%d]'s number is %d \n", order, share_mem->rx_info[order].online_count);
							broadSend_s.rx_info_s.online_count = share_mem->rx_info[order].online_count;
							if (broadSend_s.rx_info_s.online_count >= 2)
							{
								printf("RX[%d] device number more than 2, please check \n", order);
								goto Send;
							}
						}
						broadSend_s.uuid = broadRecv_s.uuid;
						switch (broadRecv_s.ucSignal)
						{
							case HEART:
								share_mem->rx_info[order].heart_count++;
								//printf("heart count: %d \n", share_mem->rx_info[order].heart_count);
								//broadSend_s.ucSignal = HEART;
								if (START == broadSend_s.ucSignal)
								{
									goto Send;
								}
								//printf();
								//source is chenged or tv status is changed or osd status is changed 
								if (broadRecv_s.rx_info_s.video_source != share_mem->rx_info[order].video_source)
								{
									broadSend_s.ucSignal = START;
									controlDataUpdate(order);
									printf("RX[%d] video sourve is changed \n");
									goto Send;
								}
								if (broadRecv_s.rx_info_s.tv_status != share_mem->rx_info[order].tv_status)
								{
									broadSend_s.ucSignal = START;
									controlDataUpdate(order);
									printf("RX[%d] tv status is changed \n");
									goto Send;
								}
								if (broadRecv_s.rx_info_s.osd_status != share_mem->rx_info[order].osd_status)
								{
									broadSend_s.ucSignal = START;
									controlDataUpdate(order);
									printf("RX[%d] osd status is changed \n");
									goto Send;
								}
								continue;
								break;
							case START:
								printf("current state is START \n");
								
								controlDataUpdate(order);
								
								broadSend_s.ucSignal = START;
								break;
							case RESPOND:
								broadSend_s.ucSignal = HEART;
								controlDataClean(order);
								break;
							default:
								printf("\n******signal error \n");
								break;

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
				printf("sendto succeed \n");
			}
		}
	}
	return 0;
}

