#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sl_uart.h>
#include <arpa/inet.h>

#include <poll.h>

#include <sys/select.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include "app_rx_io_ctl.h"
#include "display.h"
#include "app_rx_uart.h" 
#include "sharemem.h"


//#define UART_DEVICE_NAME    "/dev/ttyDMA0"
//#define UART_DEVICE_NAME    "/dev/ttyAMA0"
#define UART_DEVICE_NAME    "/dev/ttyAMA1"
//#define UART_DEVICE_NAME    "/dev/ttyAMA2"

#define UART_PORT	8810
#define BROD_PORT	8888
#define UDP_UART    
//#define TCP_UART          //jason

pthread_mutex_t lock_kvm; 
static SL_U32 fd;  			//fd is uart descriptor
char RX_IP[30];
extern char multicast[20];
extern char report_succeed;
extern char web_flag;
extern char rtp_switch_flag;

int x,y;   	//osd coordinate
int page_c=1;
int flag_dm=0;
int flag_osd=0;	//osd flag
int flag_osd_choose=0;
OSD_MSG *lhead=NULL;

char kvm_switch_flag;
char osd_display_flag;
char MK_multicast[][20] = {"239.255.42.01","239.255.42.02",\
"239.255.42.03","239.255.42.04","239.255.42.05","239.255.42.06",\
"239.255.42.07","239.255.42.08","239.255.42.09","239.255.42.10"};
typedef enum
{
	STATE0,
	STATE1,
	STATE2,
	STATE3,
	STATE4,
	STATE5,
	STATE6,
}STATE;

SL_ErrorCode_t SLUART_Setopt(SL_S32 fd, 
                             SL_U32 nSpeed, 
                             SL_U32 nBits, 
                             SL_U8  nEvent, 
                             SL_U8  nStop)
{
    struct termios newtio;
    struct termios oldtio;

    if (tcgetattr(fd, &oldtio) != 0)
    {
        return SL_ERROR_GENERIC_IO;
    }
    
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL |CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch(nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
        default:
            newtio.c_cflag |= CS8;
            break;
    }

    switch(nEvent)
    {
        case 'O':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
           case 'E':
            newtio.c_iflag |= (INPCK |ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
             break;
        case 'N':
             newtio.c_cflag &= ~PARENB;
             break;
        default:
             newtio.c_cflag &= ~PARENB;
            break;
    }
 
    switch(nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 230400:
            cfsetispeed(&newtio, B230400);
            cfsetospeed(&newtio, B230400);
            break;
        case 460800:
            cfsetispeed(&newtio, B460800);
            cfsetospeed(&newtio, B460800);
            break;
        case 921600:
            cfsetispeed(&newtio, B921600);
            cfsetospeed(&newtio, B921600);
            break;
        case 1000000:
            cfsetispeed(&newtio, B1000000);
            cfsetospeed(&newtio, B1000000);
            break;
        case 1152000:
            cfsetispeed(&newtio, B1152000);
            cfsetospeed(&newtio, B1152000);
            break;
        case 1500000:
            cfsetispeed(&newtio, B1500000);
            cfsetospeed(&newtio, B1500000);
            break;
        case 2000000:
            cfsetispeed(&newtio, B2000000);
            cfsetospeed(&newtio, B2000000);
            break;
        case 2500000:
            cfsetispeed(&newtio, B2500000);
            cfsetospeed(&newtio, B2500000);
            break;
        case 3000000:
            cfsetispeed(&newtio, B3000000);
            cfsetospeed(&newtio, B3000000);
            break;
        case 3500000:
            cfsetispeed(&newtio, B3500000);
            cfsetospeed(&newtio, B3500000);
            break;
        case 4000000:
            cfsetispeed(&newtio, B4000000);
            cfsetospeed(&newtio, B4000000);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
           break;
    }

    if (nStop == 1)
    {
         newtio.c_cflag &= ~CSTOPB;
    }
    else if (nStop == 2)
    {
           newtio.c_cflag |= CSTOPB;
    }
    else
    {
         newtio.c_cflag &= ~CSTOPB;
    }

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH);

    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        return SL_ERROR_GENERIC_IO;
    }

    return SL_NO_ERROR;
}

SL_ErrorCode_t SLUART_Open(const SLUART_OpenParams_t *OpenParams)
{
    if (OpenParams == NULL) 
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    
    fd = open (UART_DEVICE_NAME, O_RDWR);
    //printf("----------fd = %d \n", fd);    //Jason test
    if (fd < 0)
    {
        return SL_ERROR_UNKNOWN_DEVICE;
    }           
   
    if((SLUART_Setopt(fd, OpenParams->speed, OpenParams->bits,
                      OpenParams->event,OpenParams->stop)) != SL_NO_ERROR)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    return SL_NO_ERROR; 
}

SL_ErrorCode_t SLUART_Close()
{
    if ((close(fd)) < 0)
    {
		return  SL_ERROR_INVALID_HANDLE; 
    }
    return SL_NO_ERROR;
}

SL_ErrorCode_t SLUART_Read(SL_U8 *ReadBuffer, SL_U32 ReadLength)
{

    if (ReadBuffer == NULL || ReadLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((read(fd, ReadBuffer, ReadLength)) < 0)
	{
        return SL_ERROR_GENERIC_IO;
    }
	return SL_NO_ERROR;
}

SL_ErrorCode_t SLUART_Write(SL_U8 *WriteBuffer, SL_U32 WriteLength)
{

    if (WriteBuffer == NULL || WriteLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    if ((write(fd, WriteBuffer, WriteLength)) < 0)
    {
		return SL_ERROR_GENERIC_IO;
    }
    return SL_NO_ERROR;
}

void uart_init(void)
{
	
}

#if 1
/*
 * func:judge mouse and keyboard values
 * author:wgg
 */
int Judge_MK_Value(SL_U8 buff[1],STATE cur_state)
{
	char dest[3] = {""};
	char com_multicast[]="239.255.42.";
	int num;
	switch(cur_state)
	{
		case STATE0: //
			if(buff[0]==0x57)
				cur_state=STATE1;
			else
				cur_state=STATE0;
			break;
		case STATE1:
			if(buff[0]==0xab)
				cur_state=STATE2;
			else
				cur_state=STATE0;
			break;
		case STATE2:
			if(buff[0]==0x01)
				cur_state=STATE3;
			else
				cur_state=STATE0;
			break;
		case STATE3:
			if(buff[0]==0x04 || buff[0]==0x40)
				cur_state=STATE4;
			else
				cur_state=STATE0;
			break;
		case STATE4:
			if(buff[0]==0x00)
				cur_state=STATE5;
			else
				cur_state=STATE0;
			break;
		case STATE5:
			printf("STATE5");
			if(buff[0]==0x59 || buff[0]==0x1e)			//1
			{
				strcpy(multicast, MK_multicast[0]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5a || buff[0]==0x1f)		//2
			{
				strcpy(multicast, MK_multicast[1]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5b || buff[0]==0x20)		//3
			{
				strcpy(multicast, MK_multicast[2]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5c || buff[0]==0x21)		//4
			{
				strcpy(multicast, MK_multicast[3]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5d || buff[0]==0x22)		//5
			{
				strcpy(multicast, MK_multicast[4]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5e || buff[0]==0x23)		//6
			{
				strcpy(multicast, MK_multicast[5]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5f || buff[0]==0x24)		//7
			{
				strcpy(multicast, MK_multicast[6]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x60 || buff[0]==0x25)		//8
			{
				strcpy(multicast, MK_multicast[7]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x61 || buff[0]==0x26)		//9
			{
				strcpy(multicast, MK_multicast[8]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x62 || buff[0]==0x27)		//0
			{
				strcpy(multicast, MK_multicast[9]);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
#if 0
			else if(buff[0]==0x56 || buff[0]==0x2d)		//-
			{
				strncpy(dest, multicast+11,3);
				num=atoi(dest);
				num--;
				sprintf(dest,"%d",num);
				strcat(com_multicast,dest);
				strcpy(multicast,com_multicast);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x57 || buff[0]==0x2e)		//+
			{
				strncpy(dest, multicast+11,3);
				num=atoi(dest);
				num++;
				sprintf(dest,"%d",num);
				strcat(com_multicast,dest);
				strcpy(multicast,com_multicast);
				kvm_switch_flag=1;
				osd_display_flag=1;
				cur_state=STATE6;
			}
#endif
			else
				cur_state=STATE0;
			break;

		default:
			cur_state=STATE0;
		}
		return cur_state;
}
#endif


/*Function name	:Judge_OSD_CHOOSE
 * func						:Judge osd selection button
 * */
int Judge_OSD_CHOOSE(SL_U8 buff[1],STATE osd_state)
{	
	OSD_MSG *p=lhead->next;
	char com_multicast[]="239.255.42.";
	switch(osd_state)
	{
		case STATE0:
			if(buff[0]==0x57)
				osd_state=STATE1;
			else
				osd_state=STATE0;
			break;
		case STATE1:
			if(buff[0]==0xab)
				osd_state=STATE2;
			else
				osd_state=STATE0;
			break;
		case STATE2:
			if(buff[0]==0x01)
				osd_state=STATE3;
			else
				osd_state=STATE0;
			break;
		case STATE3:
			if(flag_osd_choose==0)
			{
				if(buff[0]==0x04||buff[0]==0x40)
					osd_state=STATE4;
			}
			else
			{
				if(buff[0]==0x00)
					osd_state=STATE4;
				else
					osd_state=STATE0;
			}
			break;
		case STATE4:
			if(buff[0]==0x00)
				osd_state=STATE5;
			else
				osd_state=STATE0;
			break;
		case STATE5:
			if(buff[0]==0x52)											//up
			{
				if(flag_osd==1)
				{
					if(y>80 && y<=192)
					{
						osd_choose_show(x,y,ICON_CONNECT);
						y-=56;
						osd_choose_show(x,y,ICON_CONTRL);
					}
				}
				osd_state=STATE6;
			}
			else if(buff[0]==0x51)									//down
			{
				if(flag_osd==1)
				{
					if(y<192 && y>=80)
					{
						osd_choose_show(x,y,ICON_CONNECT);
						y+=56;
						osd_choose_show(x,y,ICON_CONTRL);
					}
				}
				osd_state=STATE6;
			}
			else if(buff[0]==0x50)									//left左
			{
				if(flag_osd==1)
				{
					if(x<=439 && x>160)
					{
						osd_choose_show(x,y,ICON_CONNECT);
						x-=93;
						osd_choose_show(x,y,ICON_CONTRL);
					}
				}
				osd_state=STATE6;
			}
			else if(buff[0]==0x4f)									//right右
			{
				if(flag_osd==1)
				{
					if(x<439 && x>=160)
					{
						osd_choose_show(x,y,ICON_CONNECT);
						x+=93;
						osd_choose_show(x,y,ICON_CONTRL);
					}
				}
				osd_state=STATE6;
			}
			else if(buff[0]==0x56 || buff[0]==0x2d)		// -
			{
				if(flag_osd==1)
				{
					if(page_c>1)
						page_c--;
					show_multicast();
				}
				osd_state=STATE6;
			}
			else if(buff[0]==0x57 || buff[0]==0x2e)		//  +
			{
				if(flag_osd==1)
				{
					page_c++;
					show_multicast();
				}
				osd_state=STATE6;
			}	
			else if(buff[0]==0x58 || buff[0]==0x28)		//Enter
			{
				if(flag_osd==1)
				{
					p=lhead->next;
					while(p!=NULL)
					{
						//~ printf("***x=%d,y=%d,page=%d***\n",p->x,p->y,p->page);
						//~ printf("**x=%d,y=%d,page=%d**\n",x,y,page_c);
						//~ printf("====osd_mul=%s======\n",p->osd_multicast);
						if(p->y==y+30 && p->page==page_c && p->x==x)
						{
							strcat(com_multicast,p->osd_multicast);
							//printf("====com_mul=%s======\n",com_multicast);
							strcpy(multicast,com_multicast);
							//printf("====mul=%s======\n",multicast);
							kvm_switch_flag=1;
							flag_osd_choose=0;
							process_osd_disable();
							break;
						}
						else
							p=p->next;
					}
				}	
				osd_state=STATE6;
			}
			else if(buff[0]==0x29)									//Esc				exit menu
			{
				process_osd_disable();
				flag_osd=0;
				flag_osd_choose=0;
				osd_state=STATE6;
			}
			else if(buff[0]==0x44)									//alt + F11  	display menu
			{
				flag_osd_choose=1;
				flag_osd=1;
				osd_menu_show();
				osd_state=STATE6;
			}
			else
				osd_state=STATE0;
			break;

		default:
			osd_state=STATE0;
		}
		return osd_state;
}



/*
 * func: control mouse and key by tcp or udp 
 * author: Jason chen, 2018/8/29
 */
void *app_rx_uart_main(void)
{
	printf("==========app_rx_uart_main==========\n");
    SLUART_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;
	
	SL_U8 rbuff[1];
	SL_U8 wbuff[1];
    int fd_uart = -1;

	nOpenParam = (SLUART_OpenParams_t *)malloc(sizeof(SLUART_OpenParams_t));
    memset(wbuff, 0, sizeof(wbuff));
    memset(rbuff, 0, sizeof(rbuff));
    nOpenParam->speed = 115200;
    nOpenParam->bits = 8;
    nOpenParam->event = 'N';
    nOpenParam->stop = 1;
    
ReOpen: 

    fd_uart = SLUART_Open(nOpenParam);
	if(fd_uart != 0)
	{
		printf("uart fd : %d \n", fd_uart);
		printf("SLUART_Open error\n");
		sleep(1);
		goto ReOpen;
	}

#ifdef UDP_UART 
	//socker set
    int sock_client;
    struct sockaddr_in server_addr,client_addr;
    int ret = 0;
    socklen_t clielen_addr_length;  
    clielen_addr_length = sizeof(client_addr);  

ReSocket:
	web_flag = 0;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UART_PORT);
    //server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_addr.s_addr = inet_addr(multicast);
	//printf(multicast);
	sock_client = socket(AF_INET, SOCK_DGRAM, 0); //UDP
	if (sock_client < 0)
    {
        printf("uart Create Socket Failed!\n");
        perror("socket");
        sleep(1);
        goto ReSocket;
    }
	//printf(multicast);
    printf("kvm server socket create ok, sock_client is = %d\n", sock_client);

#if 1
	//set Multicast loop
	int loop = 0;    //1:on  0:off
	ret = setsockopt(sock_client,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop));
	if(ret < 0)
	{
		printf("kvm set multicast loop error \n");
		perror("setsockopt");
	}
#endif


#if 1
    struct timeval timeout;
	int val;
    timeout.tv_sec = 0;                 //设置超时
    timeout.tv_usec = 5000;
    
    val = setsockopt(sock_client,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
    val = setsockopt(sock_client,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
#endif
	printf("start while\n");
	STATE cur_state=STATE0;
	STATE osd_state=STATE0;
	
	int num;
	SL_U8 package[2]; 
	strncpy(RX_IP, share_mem->sm_eth_setting.strEthIp+10,3);
	num=atoi(RX_IP);
	package[0]=num;
	int ban;
	while (1)
    {
		usleep(1000);
ReRecv:
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));
		if (1 == rtp_switch_flag)
		{
			printf("\n\n ******************KVM start socket ***********************\n\n");
			printf(multicast);
			process_osd_disable();
			flag_osd=0;
			rtp_switch_flag = 0;
			close(sock_client);				//if use kvm,must be closed first this sock
			goto ReSocket;
		}

		errCode = SLUART_Read(rbuff, sizeof(rbuff));
		if (errCode != 0)
		{
			printf("SLUART_Read error\n");
		}
		package[1]=rbuff[0];
		//~ if(rbuff[0]==0x57)
			//~ ban=12;
		//~ if(ban>1)
		//~ {
			//~ ban--;
			//~ printf("rbuff[0]=0x%x\n",rbuff[0]);
		//~ }
		//printf("====0x%x, 0x%x===\n",package[0],package[1]);
		
		cur_state = Judge_MK_Value(&rbuff[0],cur_state);
		osd_state = Judge_OSD_CHOOSE(&rbuff[0],osd_state);

		if(flag_osd==0)								//If osd switching is performed, the data will not be sent to TX
		{
			if(sendto(sock_client, package, sizeof(package), \
				0, (struct sockaddr *)&server_addr, sizeof(server_addr)) <= 0)
			{
				perror("sendto");
				printf("kvm uart send failed \n");
			}
			
			if (recvfrom(sock_client, wbuff, sizeof(wbuff), \
				0, (struct sockaddr *)&server_addr, &clielen_addr_length) <= 0)
			{
				//perror("recvfrom");
				//printf("kvm uart revfrom failed \n");
				//usleep(1000);
				goto ReRecv;
			}
		}
		errCode = SLUART_Write(wbuff, sizeof(wbuff));
		if (errCode != 0)
		{
			printf("SLUART_Write error\n");
		}

		//Redisplay the menu when clearing the broken tx
		if(flag_dm==1 && flag_osd==1)
		{
			printf("***Redisplay the menu***\n");
			sleep(1);
			osd_menu_show();
			flag_dm=0;
		}
		
    }
    close(sock_client);

#endif

#ifdef TCP_UART

	//socker 
    int sock_client;
    struct sockaddr_in servaddr;
    socklen_t servlen_addr_length;
    
    servlen_addr_length = sizeof(servaddr);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(UART_PORT);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");   //TCP



ReSocket:
    sock_client = socket(AF_INET,SOCK_STREAM, 0);  //TCP
    if (sock_client < 0)
    {
        printf("uart Create Socket Failed!\n");
        perror("socket");
        close(sock_client);
        sleep(1);
        goto ReSocket;
    }
    printf(" kvm socket ok , sock_client is %d\n", sock_client);
	
	struct timeval timeout;
    timeout.tv_sec = 2;                 //设置3s超时
    timeout.tv_usec = 0;
	
	val = setsockopt(sock_client,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
    val = setsockopt(sock_client,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
	 
    if (connect(sock_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
		perror("connect");
		close(sock_client);
		sleep(1);
		goto ReSocket;
    }
    printf("kvm connect ok \n");
	while(1)
	{
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));
		errCode = SLUART_Read(rbuff, sizeof(rbuff));
		if (errCode != 0)
		{
			printf("SLUART_Read error\n");
		}
			
		len = send(sock_client, rbuff, sizeof(rbuff),0);  
		if(len <= 0)
		{
			//perror(send);
			printf("Send data len <= 0 len= %d \n",len);
			close(sock_client);	
			sleep(1);
			goto ReSocket;
		} 
		len = recv(sock_client, wbuff, sizeof(wbuff), 0);
		if (len < 0)
		{
			//perror(recv);
			printf("Server Recieve Data Failed!\n");
			//close(sock_client);
			sleep(1);
		}
		errCode = SLUART_Write(wbuff, sizeof(wbuff));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			//return -1;
		}
	}
	close(sock_client);	

#endif
    
    
    errCode = SLUART_Close(); 
	if(errCode != 0)
	{
		printf("SLUART_Close error\n");
		//return -1;
	}
	
}


/*
 * Function name		:serch_tx_multicast_main
 * func			: serch all connect tx's multicast 
 * author		: wgg, 2019/4/26
 */
void *serch_tx_multicast_main(void)
{
	printf("==========serch_tx_multicast_main==========\n");
	init_sigcation();
	init_time();
	pthread_mutex_init(&lock_kvm,NULL);
	char substr[20];
	int sock_listen;
	int addrLen = sizeof(struct sockaddr_in);
	char dest[3];

	OSD_MSG *head=malloc(sizeof(struct osd_msg));
	lhead=head;
	head->flag=1;
	strcmp(head->osd_multicast,"hello");
	head->next=NULL;
	
	struct timeval timeout;
	int val;
    timeout.tv_sec = 1;                 //设置1s超时
    timeout.tv_usec = 0;

BRDRecv:
	if((sock_listen = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("socket fail\n");
		return -1;
	}
	val = setsockopt(sock_listen,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
	//int set = 1;
	//setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
	struct sockaddr_in recvAddr;
	memset(&recvAddr, 0, sizeof(struct sockaddr_in));
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(BROD_PORT);
	recvAddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock_listen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr)) == -1){
		printf("bind fail\n");
		return -1;
	}
	
	sleep(2);
	while(1)
	{
		memset(substr, 0, sizeof(substr));
		memset(dest, '\0', sizeof(dest));
		if (recvfrom(sock_listen, substr, sizeof(substr), \
				0, (struct sockaddr *)&recvAddr, &addrLen) <= 0)
		{
				//perror("recvfrom");
				//printf("multicast revfrom failed \n");
				//goto BRDRecv;		
		}
		//printf("\n****%s*****\n",substr);
		strncpy(dest, substr+11,3);
		
/* Clear the flag every second, 
 * After ten seconds, if there is a flag or 0, remove the multicast.
 * */
		//printf("**dest==%s*****\n",dest);
			if(find_multicast(&dest)==0)
			{
				add_multicast(&dest);
			}
			//~ else
			//~ {
				//~ sleep(1);
				//~ clear_flag();
			//~ }
			usleep(1000);
	}
	close(sock_listen);
	return 0;
}

/*Function name	:osd_menu_show
 * func					:Show osd menu
 * */
void osd_menu_show()
{
	sort_multicast();
	OSD_MSG *p=lhead->next;
	OSD_MSG *q=lhead->next;
	char buff[20];
	char dest[3] = {""};
	int mul_y=110;
	int mul_x=160;
	int page=1;
	page_c=1;
	x=160,y=80;
	strncpy(dest, multicast+11,3);
	process_osd_text_solid(320,55,"OSD");
	osd_choose_show(96,68,ICON_MENU);
	while(p!=NULL)
	{
		
		if(mul_y<222)
		{
			if(mul_x<440)
			{
				if(page==1)
				{
					sprintf(buff,"TX:%s",p->osd_multicast);
					printf("osd_multicast=%s\n",p->osd_multicast);
					process_osd_text(mul_x,mul_y,buff);
				}
				p->page=page;
				p->y=mul_y;
				p->x=mul_x;
				mul_x+=93;
			}
			else
			{
				mul_y+=56;
				mul_x=160;
			}
		}
		else
		{
			mul_y=110;
			page++;
		}
		p=p->next;	
	}
	memset(buff, 0, sizeof(buff));
	
	/*When switching TX, the control interface automatically locks and displays the current signal source.*/
	while(q!=NULL)
		{
			if(atoi(q->osd_multicast)==atoi(dest))
			{
				x=q->x;
				y=q->y-30;
				osd_choose_show(x,y,ICON_CONTRL);
			}
			q=q->next;
		}
		if(q==NULL)
		{
				osd_choose_show(x,y,ICON_CONTRL);
		}

	sprintf(buff,"page:%d",page_c);
	//printf("*****%s*****\n",buff);
	process_osd_text(110,250,buff);
	process_osd_text(268,250,multicast);
}

/*Function name	:find_multicast
 * func					:Find the multicast in the linked list
 * */
int find_multicast(char *mul)
{
	/*Sometimes mul is null, 
	 * and the null value will be placed in the linked list, 
	 * causing the osd signal source to be incomplete.
	 * */
	if(strlen(mul)==0)						
		return 1;
	OSD_MSG *p=lhead->next;
	while(p!=NULL)
	{
		if(strcmp(p->osd_multicast,mul)==0)
		{
			p->flag=1;
			return 1;
		}
		p=p->next;
		
	}
	return 0;
}

/*Function name	:add_multicast
 * func					:Add multicast to the linked list
 * */
void add_multicast(char *mul)
{
	//printf("\n****multicast=%s****\n",mul);
	OSD_MSG *p=lhead;
	OSD_MSG *tmp=malloc(sizeof(struct osd_msg));

	while(p->next!=NULL)
	{
		p=p->next;
	}
	p->next=tmp;
	strcpy(tmp->osd_multicast,mul);
	tmp->flag=1;
	tmp->next=NULL;
	//printf("\n****add multicast=%s****\n",tmp->osd_multicast);
}

/*Function name	:del_multicast
 * func					:Delete multicast in the linked list
 * */
int del_multicast()
{
	pthread_mutex_lock(&lock_kvm);
	printf("*****delete multicast******\n");
	OSD_MSG *p,*q,*s;
	p=lhead->next;
	q=lhead->next->next;
	int f=0;
	while(q)
	{
		if(q->flag==0)
		{
			p->next=q->next;
			s=q;
			q=q->next;
			free(s);
			f=1;
		}
		else
		{
			printf("q->flag1=%d\n",q->flag);
			q->flag=0;
			printf("q->flag=%d\n",q->flag);
			p=p->next;
			q=q->next;
		}
	}
	if(f==1)
	{
		flag_dm=1;	
	}
	pthread_mutex_unlock(&lock_kvm);
	//printf("************\n");
	return 0;
}

/*Function name	:show_multicast
 * func					:Display multicast when turning pages
 * */
void show_multicast()
{
	sort_multicast();
	OSD_MSG *p=lhead->next;
	char buff[20];
	x=160,y=80;
	osd_choose_show(96,68,ICON_MENU);
	while(p!=NULL)
	{
		if(p->page==page_c)
		{
			sprintf(buff,"TX:%s",p->osd_multicast);
			process_osd_text(p->x,p->y,buff);
		}
		p=p->next;	
	}
	memset(buff, 0, sizeof(buff));
	osd_choose_show(160,80,ICON_CONTRL);
	sprintf(buff,"page:%d",page_c);
	process_osd_text(110,250,buff);
	process_osd_text(268,250,multicast);
}

/*Function name	:clear_flag
 * func					:Clear the flag bit to 0
 * */
int clear_flag(void)
{
	pthread_mutex_lock(&lock_kvm);
	OSD_MSG *p=lhead->next;
	while(p!=NULL)
	{
		p->flag=1;
		p=p->next;
	}
	pthread_mutex_unlock(&lock_kvm);
	return 0;
}

/*Function name	:sort_multicast
 * func					:Sort the multicast of the linked list from small to large
 * */
void sort_multicast()
{
	OSD_MSG *pre,*p,*tail,*h;
	h=lhead;
	tail=NULL;
	while(h->next != tail)
	{
		pre=h;
		p=h->next;
		while(p->next!=tail)
		{
			if (atoi(p->osd_multicast)  > atoi(p->next->osd_multicast))
			{
				pre->next=p->next;
				p->next=pre->next->next;
				pre->next->next=p;
			}
			else
				p=p->next;
			pre=pre->next;
		}
		tail=p;
	}
}

/*Function name	:init_sigcation
 * func					:Set the timing, execute the del_multicast() function every 10 seconds.
 * */
void init_sigcation()
{
	struct sigaction act;				//设置处理信号的函数
	act.sa_handler = del_multicast;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGPROF,&act,NULL);		//时间到发送SIGROF信号
}

void init_time()
{
	struct itimerval val;		//10秒后启用定时器
	val.it_value.tv_sec = 10;
	val.it_value.tv_usec = 0;
	val.it_interval.tv_sec = 20; 		//定时器间隔为20s
	val.it_interval.tv_usec = 0;
	setitimer(ITIMER_PROF, &val, NULL);
}




