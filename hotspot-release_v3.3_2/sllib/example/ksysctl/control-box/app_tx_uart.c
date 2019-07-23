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

//#define UART_DEVICE_NAME    "/dev/ttyAMA0"
#define UART_DEVICE_NAME    "/dev/ttyAMA1"
//#define UART_DEVICE_NAME    "/dev/ttyAMA2"

#define UART_PORT	8810
//#define UDP_UART   
#define TCP_UART          //Jason add

extern char serverip[128];
static SL_U32 fd;

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


/*
 * func: control mouse and key by tcp or udp 
 * author: Jason chen, 2018/8/29
 */
void *app_tx_uart_main(void)
{
    SLUART_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;

	SL_U8 rbuff[1] = {0};
    SL_U8 wbuff[1] = {0};
    SL_U32 i;
    
	//uart set
	nOpenParam = (SLUART_OpenParams_t *)malloc(sizeof(SLUART_OpenParams_t));
    memset(wbuff, 0, sizeof(wbuff));
    nOpenParam->speed = 115200;
    nOpenParam->bits = 8;
    nOpenParam->event = 'N';
    nOpenParam->stop = 1;
    
    //socker set
    int sock_server;
    struct sockaddr_in servaddr;
    struct sockaddr_in client;  //jason add
    int ret = 0;
    int len = 0;
    socklen_t clielen_addr_length;
    
    //Jason add  for select 
	fd_set  select_set;
	int maxfd = 0;
      
    clielen_addr_length = sizeof(client);  //jason add
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(UART_PORT);
    //servaddr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");
     
    
    //open uart 
    errCode = SLUART_Open(&fd, nOpenParam, UART_DEVICE_NAME);
	if(errCode != 0)
	{
		printf("SLUART_Open error\n");
		//return -1;
	}
	 
#ifdef UDP_UART
	 
ReSocket:
	//create socket
	sock_server = socket(AF_INET, SOCK_DGRAM, 0);     //UDP
	if (sock_server < 0)
	{
		printf("uart Create Socket Failed!\n");
		perror("socket");
		sleep(1);
		goto ReSocket;
	}
	printf("server socket create ok, sock_server is = %d\n", sock_server);
	//bind
	if (bind(sock_server, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind");
		printf("Server Bind Port: %d Failed!\n", UART_PORT);
		sleep(1);
		close(sock_server);
		goto ReSocket;
	}
	printf("server bind ok \n");
	
    struct timeval timeout;

    timeout.tv_sec = 3;                 //设置3s超时
    timeout.tv_usec = 0;
    
    ret = setsockopt(sock_server,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
    if(ret < 0)
    {
		printf(" SO_SNDTIMEO time out setting failed\n");
	}
    ret = setsockopt(sock_server,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    if(ret < 0)
    {
		printf(" SO_RCVTIMEO time out setting failed\n");
	}

	
	while (1)
    {
		if (recvfrom(sock_server, wbuff, sizeof(wbuff), \
			0, (struct sockaddr_in *) &client, &clielen_addr_length) <= 0)    //jason add
		{
			perror("recvfrom");
			printf("uart revfrom failed \n");
			goto ReSocket;
		}
		else
			//printf("jason test recvfrom OK ****************\n");
			//printf("jason revfrom is : [%x]\n", wbuff[0]);		

#if 1

		errCode = SLUART_Write(fd, wbuff, sizeof(wbuff));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			return -1;
		}
		//printf("jason SLUART_Write is : [%x]\n", wbuff[0]);     //jason test
#endif	

		
		errCode = SLUART_Read(fd, rbuff, sizeof(rbuff));
		if (errCode != 0)
		{
			printf("SLUART_Read error\n");
		}
	//	printf(" jason read TX rbuff back is = [%x] \n", rbuff[0]);
		
	
		if (sendto(sock_server, rbuff, sizeof(rbuff), \
			0, (struct sockaddr *)&client, clielen_addr_length) <= 0)
		{
			perror("sendto");
			printf("uart send failed \n");
			//sleep(1);
			goto ReSocket;
		}
		//printf("jason  TX test sendto OK ****************\n");
		//printf(" jason sendto RX  back is = [%x] \n", rbuff[0]);
			
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));	
    }  
    
#endif   

#ifdef TCP_UART

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
	
ReListen:
	// listen
	if (listen(sock_server, 20))
	{
        perror("listen");
        close(sock_server);
        goto ReSocket;
	}
	printf("listen ok\n");
	
	int new_server_socket = accept(sock_server, (struct sockaddr*)&client, &clielen_addr_length);
	if (new_server_socket < 0)
	{
		printf("Server Accept Failed!\n");		
		perror("accept");
		close(sock_server);
		close(new_server_socket);		
		goto ReSocket;
	}
	printf("kvm Server Aceept OK \n");
	
	struct timeval timeout;
    timeout.tv_sec = 2;                 //设置2s超时
    timeout.tv_usec = 0;
    
    ret = setsockopt(new_server_socket,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));  //set connet timeout
    ret = setsockopt(new_server_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    
    if(ret < 0)
    {
		printf("time out setting failed\n");
	}
	
	while(1)
	{
		memset(rbuff, 0, sizeof(rbuff));
		memset(wbuff, 0, sizeof(wbuff));
		
		//recv data
		//printf("recv \n");
		len = recv(new_server_socket, wbuff, sizeof(wbuff), 0);
		if (len < 0)
		{
			printf("Server Recieve Data Failed!\n");
			close(sock_server);
			close(new_server_socket);		
			sleep(1);
			goto ReSocket;
		}		
		errCode = SLUART_Write(fd, wbuff, sizeof(wbuff));
		if(errCode != 0)
		{
			printf("SLUART_Write error\n");
			//return -1;
		}
		//printf("jason read TX wbuff back is : [%x]\n", wbuff[0]);     //jason test		
		errCode = SLUART_Read(fd, rbuff, sizeof(rbuff));
		if (errCode != 0)
		{
			printf("SLUART_Read error\n");
		}
		//printf(" jason read TX rbuff is = [%x] \n", rbuff[0]);
		len = send(new_server_socket, rbuff, sizeof(rbuff),0);  
		if(len <= 0)
		{
			printf("Send data len <= 0 len= %d \n",len);
			close(sock_server);
			close(new_server_socket);		
			sleep(1);
			goto ReSocket;
		}
		//printf(" jason send to  RX rbuff back is = [%x] \n", rbuff[0]);			
	}
#endif
	
	close(sock_server);
	close(new_server_socket);   
    errCode = SLUART_Close(fd);
	if(errCode != 0)
	{
		printf("SLUART_Close error\n");
		//return -1;
	}

}

