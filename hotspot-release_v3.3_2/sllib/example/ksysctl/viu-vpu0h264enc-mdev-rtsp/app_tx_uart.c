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
#include <pthread.h>
#include <time.h>

//#define UART_DEVICE_NAME    "/dev/ttyAMA0"
#define UART_DEVICE_NAME    "/dev/ttyAMA1"
//#define UART_DEVICE_NAME    "/dev/ttyAMA2"

#define UART_PORT	8810
#define UDP_UART   
//#define TCP_UART          //Jason add

static SL_U32 fd;
extern char serverip[128];
extern char multicast[20];
extern char web_flag;
#ifdef TCP_UART
SL_U8 rebuff[1] = {0};    
pthread_mutex_t lock_kvm; 
#endif
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


int IsLinkDownOrUp(void)
{
	char pathName[30] = {"/sys/class/net/eth0/carrier"};
	static int pFile = 0; 
	int len;
	char state[1] = {0};
	
	if(!pFile)
	{
		if ((pFile = fopen(pathName, "r")) == NULL) 
		{
			printf("can't open %s",pathName);
			return -1;
		}   
		printf("open secceed \n");
	}
	
	fseek(pFile, SEEK_SET, 0); 
	
	if ((len = fread((char *)&state, 1, 1, pFile)) < 0)
	{
		printf("fread len = %d \n", len);
		perror(fread);
		return -1;
	}
#if 0
	if (close(pFile) < 0)
	{
		printf("close pFile fail \n");
		perror(close);
		return -1;
	}
#endif
	//printf("state : %s \n", state[0]);
	
	//if (strcmp(state, "1"))
	if ('1' == state[0])
	{
		//printf("eth0 link is up \n");
		return 1;
	}
	else
	{
		printf("eth0 link is down \n");
		return 0;
	}
}

#ifdef TCP_UART
/*
 *func:Thread processing receives information from the client 
 * 		key select
 *author:wgg
 *time:2018.12.12
 */
void handlermsg(int *sfd)
{
	
	printf("handlermsg\n");
	int connfd=*sfd;
	int len;
	int j=0;
	SL_U8 rbuff[1] = {0};
	SL_U8 wbuff[1] = {0};	
	SL_ErrorCode_t errCode;
	while(1)
	{	
Rerecv:		
		len=recv(connfd, rebuff, sizeof(rebuff), 0);
		if (len<=0)
		{
			perror(recv);
			j++;
			if(j>10)				//Exit the thread if the client disconnects or doesn't receive data ten times
			pthread_exit(NULL);
			printf("Server Recieve ask Data Failed!\n");
			pthread_mutex_unlock(&lock_kvm);
			goto Rerecv;		
		}
		else
			j=0;
		if(rebuff[0]==0x57)
		{
			pthread_mutex_lock(&lock_kvm);
			memset(rebuff, 0, sizeof(rebuff));
			printf("%d lock\n",connfd);
			send(connfd,"0",1,0);
			while(1)
			{	
				memset(rbuff, 0, sizeof(rbuff));
				memset(wbuff, 0, sizeof(wbuff));
				len = recv(connfd, wbuff, sizeof(wbuff), 0);
				if (len <=0)
				{
					//perror(recv);
					pthread_mutex_unlock(&lock_kvm);
					printf("Server Recieve Data Failed!\n");						
					goto Rerecv;
				}

				errCode = SLUART_Write(wbuff, sizeof(wbuff));
				if(errCode != 0)
				{
					printf("SLUART_Write error\n");
					return -1;
				}
				errCode = SLUART_Read(rbuff, sizeof(rbuff));	
				if (errCode != 0)
				{
					printf("SLUART_Read error\n");
				}
				len=send(connfd, rbuff, sizeof(rbuff),0);
				if(len <= 0)
				{
					//perror(send);
					printf("Send data len <= 0 len= %d \n",len);;
				}
				if(rebuff[0]==0x57)
				{
					pthread_mutex_unlock(&lock_kvm);
					printf("%d exit\n",connfd);
					sleep(1);
					break;
				}
			}
		}
		send(connfd,"0",1,0);
	}					
}

#endif

/*
 * func: control mouse and key by tcp or udp 
 * author: Jason chen, 2018/8/29
 */
void *app_tx_uart_main(void)
{
    SLUART_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;
	//uart set
	nOpenParam = (SLUART_OpenParams_t *)malloc(sizeof(SLUART_OpenParams_t));
    nOpenParam->speed = 115200;
    nOpenParam->bits = 8;
    nOpenParam->event = 'N';
    nOpenParam->stop = 1;
     
    SL_U8 rbuff[1];
	SL_U8 wbuff[1];
    //open uart 
    errCode = SLUART_Open(nOpenParam);
	if(errCode != 0)
	{
		printf("SLUART_Open error\n");
		//return -1;
	}
	 
#ifdef UDP_UART
	 //socker 
    int sock_client;
    struct sockaddr_in servaddr;
    socklen_t servlen_addr_length;
    servlen_addr_length = sizeof(servaddr);
    
ReSocket: 
	web_flag = 0;   
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(UART_PORT);
    servaddr.sin_addr.s_addr = inet_addr(multicast);

	//create socket
	sock_client = socket(AF_INET,SOCK_DGRAM, 0); //UDP
    if (sock_client < 0)
    {
		perror("socket");
		sleep(1);
		goto ReSocket;
	}
	printf("sock_client = %d\n", sock_client);
//	int reuse = 1;
//	setsockopt(sock_client, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));

    struct timeval timeout;
	int val;
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
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));

		errCode = SLUART_Read(rbuff, sizeof(rbuff));
		if (errCode != 0)
		{
			printf("SLUART_Read error\n");
		}
			
		if (sendto(sock_client, rbuff, sizeof(rbuff), \
			0, (struct sockaddr *)&servaddr, servlen_addr_length) <= 0)
		{
			perror("sendto");
			printf("uart send failed \n");
			close(sock_client);
			//sleep(1);
			goto ReSocket;
		}
		
		if (recvfrom(sock_client, wbuff, sizeof(wbuff), \
			0, (struct sockaddr_in *) &servaddr, &servlen_addr_length) <= 0) 
		{
			perror("recvfrom");
			printf("uart revfrom failed \n");
			close(sock_client);
			goto ReSocket;
		}	
		
		errCode = SLUART_Write(wbuff, sizeof(wbuff));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			return -1;
		}
			
			
    }  
    close(sock_client);
#endif   

#ifdef TCP_UART
	pthread_mutex_init(&lock_kvm,NULL);
//socker set
    int sock_server;
    struct sockaddr_in servaddr;
    struct sockaddr_in client;  //jason add
    int cfd;	//保存客户端套接字
    pthread_t tid=0;	//保存线程标识符
    //int ret = 0;
    socklen_t clielen_addr_length;
    
    //Jason add  for select 
	fd_set  select_set;
      
    clielen_addr_length = sizeof(client);  //jason add
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(UART_PORT);
    //servaddr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");


ReSocket:
	if (IsLinkDownOrUp())
	printf("start creat socket \n");
	sock_server = socket(AF_INET, SOCK_STREAM, 0); //TCP
	if (sock_server < 0)
	{
		printf("Create Socket Failed!\n");
		goto ReSocket;
	}
	printf("creat socket ok \n");
	//bind
	if (bind(sock_server, (struct sockaddr*)&servaddr, sizeof(servaddr)))
	{
        perror("bind");
        close(sock_server);
        sleep(1);
        goto ReSocket;
	}
	printf("kvm bind ok\n");
	if (listen(sock_server, 20))
	{
		perror("listen");
		close(sock_server);
		goto ReSocket;
	}
	printf("kvm listen ok\n");
	while(1)
	{	
		FD_ZERO(&select_set);
		FD_SET(sock_server,&select_set);
		select(sock_server+1,&select_set,NULL,NULL,NULL);
		if(FD_ISSET(sock_server,&select_set))
		{
			cfd = accept(sock_server, (struct sockaddr*)&client, &clielen_addr_length);
			if (cfd < 0)
			{
				printf("Server Accept Failed!\n");		
				perror("accept");
				close(sock_server);
				close(cfd);		
				goto ReSocket;
			}
			else
			{
				printf("accept success!cfd=%d\n",cfd);
				pthread_create(&tid,NULL,(void *)handlermsg,(void *)&cfd);	
				pthread_detach(tid);
				continue;			
			}
		}					
	}
	pthread_mutex_destroy(&lock_kvm);
	close(sock_server);
	close(cfd); 
#endif  
	close(sock_client);  
    errCode = SLUART_Close();
	if(errCode != 0)
	{
		printf("SLUART_Close error\n");
		//return -1;
	}

}

