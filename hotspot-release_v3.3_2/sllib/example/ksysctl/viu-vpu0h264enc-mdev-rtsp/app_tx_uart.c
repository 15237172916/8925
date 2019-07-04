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
#define BROD_PORT	8888
#define UDP_UART   
//#define TCP_UART          //Jason add

static SL_U32 fd;
extern char serverip[128];
extern char multicast[20];
extern char web_flag;
SL_U8 package[2]; 

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
	int x;
    if (WriteBuffer == NULL || WriteLength < 0)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    x=write(fd, WriteBuffer, WriteLength);
    if (x < 0)
    {
		return SL_ERROR_GENERIC_IO;
    }
    if(*WriteBuffer > 0)
    {
      //printf("pcBuffer=0x%x\n",*WriteBuffer);
    }
    return SL_NO_ERROR;
}
/*
 *func:send ctrl value to TX,solve key value incomplete problem
 * 		
 *author:llc
 *time:2019.5.7
 */
void SLUART_Write_Kvmend(void)
{
	SL_U8  kvmend_bit[1]={0};
	SL_ErrorCode_t errCode;
	int a=0,b=0;
	//sleep(5);
	while(a!=10)
	{
		kvmend_bit[0]=0x00;
		errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			//return -1;
		}
		a++;
	}
	a=0;
	kvmend_bit[0]=0x57;
	errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
	if(errCode != 0)
	{
		printf("SLUART_Write error\n");
		return -1;
	}
	kvmend_bit[0]=0xab;
	errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
	if(errCode != 0)
	{
		printf("SLUART_Write error\n");
		//return -1;
	}
	kvmend_bit[0]=0x01;
	errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
	if(errCode != 0)
	{
		printf("SLUART_Write error\n");
		//return -1;
	}
	while(a!=10)
	{
		kvmend_bit[0]=0x00;
		errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			//return -1;
		}
		a++;
	}


	while(b!=10)
	{
		kvmend_bit[0]=0x00;
		errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			//return -1;
		}
		b++;
	}
	b=0;
	kvmend_bit[0]=0x57;
	errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
	if(errCode != 0)
	{
		printf("SLUART_Write error\n");
		//return -1;
	}
	kvmend_bit[0]=0xab;
	errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
	if(errCode != 0)
	{
		printf("SLUART_Write error\n");
		//return -1;
	}
	kvmend_bit[0]=0x02;
	errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
	if(errCode != 0)
	{
		printf("SLUART_Write error\n");
		//return -1;
	}
	while(b!=10)
	{
		kvmend_bit[0]=0x00;
		errCode = SLUART_Write(kvmend_bit, sizeof(kvmend_bit));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			//return -1;
		}
		b++;
	}
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
				memset(rbuff,0, sizeof(rbuff));
				memset(wbuff,0, sizeof(wbuff));
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



#if 1
/*
 * func:Determine the release of the mouse and keyboard
 * author:wgg
 */
 int i=0;
int Judge_MK_Value(SL_U8 buff[1],STATE cur_state)
{
	switch(cur_state)
	{
		case STATE0: 
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
				if(buff[0]==0)
				{
					//printf("*********\n");
					i++;
					cur_state=STATE3;
					if(i>10)
					{
						i=0;
						SLUART_Write_Kvmend();
						cur_state=STATE0;
						//printf("=======\n");
					}
				}
				else
					cur_state=STATE0;
				break;
		default:
			cur_state=STATE0;
		}
		return cur_state;
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
	char SLUART_Write_Kvmend_end;
	char SLUART_Write_Kvmend_start;
    //open uart 
    errCode = SLUART_Open(nOpenParam);
	if(errCode != 0)
	{
		printf("SLUART_Open error\n");
		//return -1;
	}
	 
#ifdef UDP_UART
	 //socker 
    int sock_server, ret;
    struct sockaddr_in servaddr;
    socklen_t servlen_addr_length;
    servlen_addr_length = sizeof(servaddr);

ReSocket:
	web_flag = 0; 
	SLUART_Write_Kvmend_end=0;
	SLUART_Write_Kvmend_start=0;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(UART_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //servaddr.sin_addr.s_addr = inet_addr(multicast);

	//create socket
	sock_server = socket(AF_INET,SOCK_DGRAM, 0); //UDP
    if (sock_server < 0)
    {
		perror("socket");
		sleep(1);
		goto ReSocket;
	}
	printf("sock_server = %d\n", sock_server);

	if (bind(sock_server, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind");
		printf("Server Bind Port: %d Failed!\n", UART_PORT);
		close(sock_server);
		sleep(1);
		goto ReSocket;
	}
    printf("bind socket ok \n");

#if 1
	//set Multicast loop
	int loop = 0;    //1:on  0:off
	ret = setsockopt(sock_server, IPPROTO_IP, IP_MULTICAST_LOOP,&loop,sizeof(loop));
	if(ret < 0)
	{
		printf("kvm set multicast loop error \n");
		perror("setsockopt");
	}

	//set multicast address
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicast);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	//add multicast group
	ret = setsockopt(sock_server, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if(ret < 0)
	{
		printf("kvm set multicast error \n");
		perror("setsockopt");
	}
	
	int reuse = 1;
	ret = setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
	if(ret < 0)
	{
		printf("kvm set multicast reuse error \n");
		perror("setsockopt");
	}
#endif

    struct timeval timeout;
	int val;
    timeout.tv_sec = 0;                 //设置3s超时
    timeout.tv_usec = 400000;
    
    val = setsockopt(sock_server,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
    val = setsockopt(sock_server,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(val < 0)
    {
		printf("time out setting failed\n");
	}
	
	STATE cur_state=STATE0;
	while (1)
    {
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));
		memset(package, 0, sizeof(package));
		if (recvfrom(sock_server, package, sizeof(package), \
			0, (struct sockaddr_in *) &servaddr, &servlen_addr_length) <= 0)
		{
			perror("recvfrom");
			printf("uart revfrom failed \n");
			//SLUART_Write_Kvmend_start=0;
			SLUART_Write_Kvmend_end++;
			if(SLUART_Write_Kvmend_end==1)
			{
				int send_frequency_end=7;
				while(send_frequency_end)
				{
					SLUART_Write_Kvmend();
					send_frequency_end--;
				}
			}
			SLUART_Write_Kvmend_end=2;
			//close(sock_server);
			//goto ReSocket;
		}
		else
		{
			SLUART_Write_Kvmend_end=0;
	    }

		//printf("====0x%x, 0x%x===\n",package[0],package[1]);
		wbuff[0]=package[1];
		cur_state = Judge_MK_Value(&wbuff[0],cur_state);
		errCode = SLUART_Write(wbuff, sizeof(wbuff));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			//return -1;
		}
		

		errCode = SLUART_Read(rbuff, sizeof(rbuff));
		if (errCode != 0)
		{
			printf("SLUART_Read error\n");
		}
			
		if (sendto(sock_server, rbuff, sizeof(rbuff), \
			0, (struct sockaddr *)&servaddr, sizeof(servaddr)) <= 0)
		{
			perror("sendto");
			printf("uart send failed \n");
			//close(sock_server);
			//sleep(1);
			//goto ReSocket;
		}
    }
    close(sock_server);
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
	close(sock_server);
    errCode = SLUART_Close();
	if(errCode != 0)
	{
		printf("SLUART_Close error\n");
		//return -1;
	}

}


/*
 * func: broadcast send tx's multicast 
 * author: wgg, 2019/4/26
 */
void *send_tx_multicast_main(void)
{
	sleep(5);
	printf("==========send_tx_multicast_main==========\n");
	int brd_sock;
	int ret;
	if((brd_sock = socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		printf("send tx broadcast fail!\n");
		return -1;
	}
	
	int optval = 1;//这个值一定要设置，否则可能导致sendto()失败
	setsockopt(brd_sock, SOL_SOCKET, SO_BROADCAST , &optval, sizeof(optval));
	struct sockaddr_in brdAddr;
	memset(&brdAddr, 0, sizeof(struct sockaddr_in));	
	brdAddr.sin_family = AF_INET;
	brdAddr.sin_addr.s_addr =htonl(INADDR_BROADCAST);
	brdAddr.sin_port = htons(BROD_PORT);
	
	while(1)
	{
		//printf("==multicast=%s===\n",multicast);
		if(sendto(brd_sock, multicast, sizeof(multicast), \
			0, (struct sockaddr *)&brdAddr, sizeof(brdAddr)) <= 0)
		{
			perror("sendto");
			printf("multicast send failed \n");
		}
		sleep(2);
	}
	close(brd_sock);
	return 0;
}

