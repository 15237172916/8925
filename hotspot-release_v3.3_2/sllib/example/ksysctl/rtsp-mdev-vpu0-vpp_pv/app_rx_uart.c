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
#include <sys/time.h>
#include <unistd.h>
#include "app_rx_io_ctl.h"


//#define UART_DEVICE_NAME    "/dev/ttyDMA0"
//#define UART_DEVICE_NAME    "/dev/ttyAMA0"
#define UART_DEVICE_NAME    "/dev/ttyAMA1"
//#define UART_DEVICE_NAME    "/dev/ttyAMA2"

#define UART_PORT	8810
#define UDP_UART    
//#define TCP_UART          //jason


static SL_U32 fd;  			//fd is uart descriptor
extern char serverip[128];
extern char multicast[20];
extern char report_succeed;
extern char web_flag;
extern char rtp_switch_flag;

char kvm_switch_flag;

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


/*
 * func: Functions handled when switching multicast addresses
 * author:wgg
 */
 /*
void Multicast_Handle(int sock_client,struct ip_mreq mreq1)
{

	setsockopt(sock_client, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq1, sizeof(mreq1));
	usleep(100);
	struct ip_mreq mreq;      
	mreq.imr_multiaddr.s_addr = inet_addr(multicast);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	setsockopt(sock_client, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	
}*/


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
			if(buff[0]==0x01 || buff[0]==0x10)
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
				cur_state=STATE6;
			}
			else if(buff[0]==0x5a || buff[0]==0x1f)		//2
			{
				strcpy(multicast, MK_multicast[1]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5b || buff[0]==0x20)		//3
			{
				strcpy(multicast, MK_multicast[2]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5c || buff[0]==0x21)		//4
			{
				strcpy(multicast, MK_multicast[3]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5d || buff[0]==0x22)		//5
			{
				strcpy(multicast, MK_multicast[4]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5e || buff[0]==0x23)		//6
			{
				strcpy(multicast, MK_multicast[5]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x5f || buff[0]==0x24)		//7
			{
				strcpy(multicast, MK_multicast[6]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x60 || buff[0]==0x25)		//8
			{
				strcpy(multicast, MK_multicast[7]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x61 || buff[0]==0x26)		//9
			{
				strcpy(multicast, MK_multicast[8]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x62 || buff[0]==0x27)		//0
			{
				strcpy(multicast, MK_multicast[9]);
				kvm_switch_flag=1;
				cur_state=STATE6;
			}
			else if(buff[0]==0x56 || buff[0]==0x2d)		//-
			{
				strncpy(dest, multicast+11,3);
				num=atoi(dest);
				num--;
				sprintf(dest,"%d",num);
				strcat(com_multicast,dest);
				strcpy(multicast,com_multicast);
				kvm_switch_flag=1;
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
				cur_state=STATE6;
			}
			else
				cur_state=STATE0;
			break;

		default:
			cur_state=STATE0;
		}
		return cur_state;
}



/*
 * func: control mouse and key by tcp or udp 
 * author: Jason chen, 2018/8/29
 */
void *app_rx_uart_main(void)
{
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
#if 0
	//set multicast address
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicast);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	//add multicast group
	ret = setsockopt(sock_client, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if(ret < 0)
	{
		printf("kvm set multicast error \n");
		perror("setsockopt");
	}
#endif
#if 0
	int reuse = 1;
	ret = setsockopt(sock_client, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
	if(ret < 0)
	{
		printf("kvm set multicast reuse error \n");
		perror("setsockopt");
	}
#endif

#if 1
    struct timeval timeout;
	int val;
    timeout.tv_sec = 0;                 //设置3s超时
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
	//printf("start while\n");
	STATE cur_state=STATE0;
	int i = 0, buf_tmp;
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
			rtp_switch_flag = 0;
			close(sock_client);				//if use kvm,must be closed first this sock
			goto ReSocket;
		}
		
		errCode = SLUART_Read(rbuff, sizeof(rbuff));
		if (errCode != 0)
		{
			printf("SLUART_Read error\n");
		}
		#if 0
		i++;
		//if (0 != *rbuff)
			printf("0x%x", *rbuff);
		
		if (20==i)
		{
			printf("\n");
			i = 0;
		}
		#endif
		//printf("0x%x \n", *rbuff);
		
		cur_state = Judge_MK_Value(&rbuff[0],cur_state);
		//buf_tmp = rbuff[0];
		//printf(multicast);
		//printf("\n");
		if(sendto(sock_client, rbuff, sizeof(rbuff), \
			0, (struct sockaddr *)&server_addr, sizeof(server_addr)) <= 0)
		{
			perror("sendto");
			printf("uart send failed \n");
		}

		if (recvfrom(sock_client, wbuff, sizeof(wbuff), \
			0, (struct sockaddr *)&server_addr, &clielen_addr_length) <= 0)
		{
			perror("recvfrom");
			printf("uart revfrom failed \n");
			goto ReRecv;
		}

		errCode = SLUART_Write(wbuff, sizeof(wbuff));
		if (errCode != 0)
		{
			printf("SLUART_Write error\n");
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

