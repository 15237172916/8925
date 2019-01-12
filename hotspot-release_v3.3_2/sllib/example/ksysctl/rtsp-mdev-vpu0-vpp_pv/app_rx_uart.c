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
 * func: control mouse and key by tcp or udp 
 * author: Jason chen, 2018/8/29
 */
void *app_rx_uart_main(void)
{
    SLUART_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;

	SL_U8 rbuff[1] = {0};
    SL_U8 wbuff[1] = {0};
    SL_U8 ret;
    SL_U32 i, tmp;
    int fd_uart = -1;
    int len = 0;
   // SL_ErrorCode_t fd_uart;   //jason

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
	//poll
    struct pollfd fds;
    fds.fd = 5;        //uart descriptor
    fds.events = POLLIN;
	
	//Jason add  for select 
	fd_set  select_set;
	int maxfd = 0;
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
#if 0
		//struct timeval timeout = {3, 0}; 						// select 等待 3 秒，3 秒轮询， 要非阻塞就置 0			
		FD_ZERO(&select_set);
		FD_SET(5, &select_set);
					
		maxfd = sock_client > fd ? sock_client +1 : fd +1;   		//fd is uart descriptor


		if(-1 == select(maxfd, &select_set, NULL, NULL,NULL))       //select for uart
		{
			perror("select");
		}
		
		if(FD_ISSET(fd, &select_set))   							//Blocking events
#endif		
		//else if ((fds.revents & POLLIN) == POLLIN)
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
    timeout.tv_sec = 2;                 //设置3s超时
    timeout.tv_usec = 0;
	
	val = setsockopt(sock_client,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
    val = setsockopt(sock_client,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
	
ReConnect: 
    if (connect(sock_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
		perror("connect");
		close(sock_client);
		sleep(1);
		goto ReSocket;
    }
    printf("kvm connect ok \n");
    //printf("uart_fd = %d \n", fd);

	//Sleep(10);
	
	while(1)
	{
#if 0
		FD_ZERO(&select_set);
		FD_SET(fd, &select_set);
					
		maxfd = sock_client > fd ? sock_client +1 : fd +1;   		//fd is uart descriptor


		if(-1 == select(maxfd, &select_set, NULL, NULL,NULL))       //select for uart
		{
			perror("select");
		}
		
		if(FD_ISSET(fd, &select_set))   							//Blocking events
#endif
		//else if ((fds.revents & POLLIN) == POLLIN)
		{
			memset(rbuff, 0, sizeof(rbuff));
		    memset(wbuff, 0, sizeof(wbuff));
			
			errCode = SLUART_Read(rbuff, sizeof(rbuff));
			if (errCode != 0)
			{
				printf("SLUART_Read error\n");
			}
			//printf(" jason read TX rbuff back is = [%x] \n", rbuff[0]);
			//printf("-0x%x", rbuff[0]);
			len = send(sock_client, rbuff, sizeof(rbuff),0);  
			if(len <= 0)
			{
				perror(send);
				printf("Send data len <= 0 len= %d \n",len);
				close(sock_client);	
				sleep(1);
				goto ReSocket;
			}
			//printf("jason test ********  sendto OK \n");
			//printf("jason1 sendto is = [%x] \n", rbuff[0]);	
			
			//recv data
			len = recv(sock_client, wbuff, sizeof(wbuff), 0);
			if (len < 0)
			{
				perror(recv);
				printf("Server Recieve Data Failed!\n");
				close(sock_client);
				sleep(1);
				goto ReSocket;
			}
			//printf("*0x%x", wbuff[0]);
			
			errCode = SLUART_Write(wbuff, sizeof(wbuff));
			if(errCode != 0)
			{
				printf("SLUART_Write error\n");
				return -1;
			}
		}
		//printf("jason read TX wbuff back is : [%x]\n", wbuff[0]);     //jason test
		
	}

#endif
    
    close(sock_client);
    errCode = SLUART_Close(); 
	if(errCode != 0)
	{
		printf("SLUART_Close error\n");
		return -1;
	}
	
}

