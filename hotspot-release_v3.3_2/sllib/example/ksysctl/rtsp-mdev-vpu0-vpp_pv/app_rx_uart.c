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
#include "sharemem.h"

//#define UART_DEVICE_NAME    "/dev/ttyDMA0"
//#define UART_DEVICE_NAME    "/dev/ttyAMA0"
#define UART_DEVICE_NAME    "/dev/ttyAMA1"
//#define UART_DEVICE_NAME    "/dev/ttyAMA2"

#define UART_PORT	8810
//#define UDP_UART    
#define TCP_UART          //jason


static SL_U32 fd;  			//fd is uart descriptor
extern char serverip[128];

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
	int i;
    if (ReadBuffer == NULL || ReadLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    i=read(fd, ReadBuffer, ReadLength);
    if (i <=0)
	{
        return SL_ERROR_GENERIC_IO;
    }
    //printf("read;%d",i);
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
 * func: control mouse and key by tcp or udp 
 * author: Jason chen, 2018/8/29
 */
void *app_rx_uart_main(void)
{
    SLUART_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;
	SL_U32 speed_flg;
	SL_U8 rbuff[3] = {0};
    SL_U8 wbuff[3] = {0};
     SL_U8 wbuff1[1] = {0};
    int fd_uart = -1;
    int len = 0;
   // SL_ErrorCode_t fd_uart;   //jason

speed_flg=share_mem->sm_eth_setting.ucspeed;
	nOpenParam = (SLUART_OpenParams_t *)malloc(sizeof(SLUART_OpenParams_t));
    memset(wbuff, 0, sizeof(wbuff));
    memset(rbuff, 0, sizeof(rbuff));
    printf("++++++++++++++++++++++++++%d+++++++++++++++++++++++++++++++\n",share_mem->sm_eth_setting.ucspeed);
    SPEED:
  switch(share_mem->sm_eth_setting.ucspeed)
    {
        case 1:
        nOpenParam->speed = 2400;
            break;
        case 2:
           nOpenParam->speed = 4800;
            break;
        case 3:
    nOpenParam->speed = 9600;
            break;
        case 4:
           nOpenParam->speed = 115200;
            break;
        case 5:
            nOpenParam->speed = 230400;
            break;
        case 6:
            nOpenParam->speed = 460800;
            break;
        case 7:
            nOpenParam->speed = 921600;
            break;
        case 8:
              nOpenParam->speed = 1000000;
            break;
        case 9:
          nOpenParam->speed = 1152000;
            break;
        case 10:
              nOpenParam->speed = 1500000;
            break;
        case 11:
             nOpenParam->speed = 2000000;
            break;
        case 12:
              nOpenParam->speed = 2500000;
            break;
        case 13:
              nOpenParam->speed = 3000000;
            break;
        case 14:
               nOpenParam->speed = 3500000;
            break;
         case 15:
              nOpenParam->speed = 4000000;
              break;
        default:
      		  nOpenParam->speed = 115200;
              share_mem->sm_eth_setting.ucspeed=4;
           break;
    }

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
	//poll
    struct pollfd fds;
    fds.fd = 5;        //uart descriptor
    fds.events = POLLIN;
	
	//Jason add  for select 
	int val = 0;

	//socker 
    int sock_client;
    struct sockaddr_in servaddr;
    socklen_t servlen_addr_length;
    
    servlen_addr_length = sizeof(servaddr);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(UART_PORT);
    //servaddr.sin_addr.s_addr = inet_addr(serverip);
    //servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");   //TCP
    
#ifdef UDP_UART 
    
ReSocket:
	sock_client = socket(AF_INET,SOCK_DGRAM, 0); //UDP
	if (sock_client < 0)
    {
        printf("uart Create Socket Failed!\n");
        perror("socket");
        sleep(1);
        close(sock_client);
        goto ReSocket;
    }
    printf("jason test sock_client = %d\n", sock_client);
    
    struct timeval timeout;

    timeout.tv_sec = 5;                 //设置3s超时
    timeout.tv_usec = 0;
    
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

	while (1)
    {
		{
			errCode = SLUART_Read(rbuff, sizeof(rbuff));
			if (errCode != 0)
			{
				printf("SLUART_Read error\n");
			}					
			//printf("jason1 rbuff is = [%x] \n", rbuff[0]);
			

			if (sendto(sock_client, rbuff, sizeof(rbuff), \
				0, (struct sockaddr *)&servaddr, sizeof(servaddr)) <= 0)
			{
				perror("sendto");
				printf("uart send failed \n");
				sleep(1);
				close(sock_client);
				goto ReSocket;
			}
			//printf("jason test ********  sendto OK \n");
			//printf("jason1 sendto is = [%x] \n", rbuff[0]);		
	
		}	
				

		if (recvfrom(sock_client, wbuff, sizeof(wbuff), \
			0, (struct sockaddr_in *) &servaddr, &servlen_addr_length) <= 0)
		{
			perror("recvfrom");
			printf("uart revfrom failed \n");
			close(sock_client);
			goto ReSocket;
		}
		//printf("jason test recvfrom ----------------OK \n");	
		//printf("Jason test  RX recvfrom is [%s]\n", wbuff);   	//jason test

		
	
		errCode = SLUART_Write(wbuff, sizeof(wbuff));
		if (errCode != 0)
		{
			printf("SLUART_Write error\n");
		}
		//printf("Jason test SLUART_Write is [%x]\n", wbuff[0]);   //jason test			
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));
    }
    
#endif

#ifdef TCP_UART

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
	timeout.tv_sec = 2;                 //设置超时
	timeout.tv_usec = 0;

	val = setsockopt(sock_client,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
	val = setsockopt(sock_client,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
    Key_Init();
	while (1)
    {
        if (1 == share_mem->sm_eth_setting.ucUartState || 0 == get_key_value())
        {
            if (connect(sock_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
            {
                perror("connect");
                close(sock_client);
                sleep(1);
                goto ReSocket;
            }
            else
            {
                printf("uart connect ok \n");
                break;
            }
        }
        else
        {
            usleep(100000);
           // printf("uart is not connecting \n");
        }
    }
	while (1)
	{
Rerecv:
        	usleep(10000);
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));
		//printf("uart read \n");
		errCode = SLUART_Read(rbuff, 1);	
		if (errCode == SL_NO_ERROR)
		{
			//rbuff[0]=0xab;			//goto Rerecv;
			rbuff[1]= 0xab;
			rbuff[2]=0xac;
			//printf("%x  ", rbuff[0]);
		}
		//printf("-----------------uart read : %s----------------\n",rbuff);
		//printf("send \n");
		len = send(sock_client, rbuff, sizeof(rbuff),0);  
		if(len <= 0)
		{
			perror(send);
			//printf("Send data len <= 0 len= %d \n",len);
			//printf("Server Recieve Data Failed!\n");		
			goto Rerecv;
		}
		
#if 1
		//printf("revc \n");
		len = recv(sock_client, wbuff, sizeof(wbuff), 0);
		if (len <= 0)
		{
			perror(recv);
			//printf("========recv error ==========\n");
			//printf("Server Recieve Data Failed!\n");		
			goto Rerecv;
		}
		//printf("************** revc: %s ******************\n", wbuff);
		if (wbuff[1]==0xab&&wbuff[2]==0xac)
		{

			// printf(" %x", wbuff[0]);
			errCode = SLUART_Write(wbuff, 1);
			if(errCode != 0)
			{
				printf("SLUART_Write error\n");
				//return -1;
			}
			
		}
		if(wbuff[1]==0xcc&&wbuff[2]==0xdd)
		{
			//printf("server exit connect \n");
			printf("lo\n");
			sleep(1);
			close(sock_client);
			goto ReSocket;
		}
#endif


	}

#endif
    
    close(sock_client);
    errCode = SLUART_Close(); 
	if(errCode != 0)
	{
		printf("SLUART_Close error\n");
		//return -1;
	}
	
}

