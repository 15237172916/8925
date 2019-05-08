#include <sys/types.h>
#include <sys/socket.h>                        
#include <stdio.h>
#include <netinet/in.h>                        
#include <arpa/inet.h>                     
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
#include <netinet/tcp.h>

#include <sys/time.h>

#include "app_tx_data_ch.h"
#include <errno.h>

#include "wifi_ap.h"

//#define DEBUG_LOG

//#define MYPORT  8001
#define BUFFER_SIZE 1024
#define DATANUM 10000000
#define LENGTH_OF_LISTEN_QUEUE     20


typedef struct
{
//    int ab;
    int num[DATANUM];
}Node;

//add parameter arg by 2017.08.30
void *app_tx_data_ch_main(void *arg)
{
	int port = *((int *)arg);
	
	//sockfd
	int server_socket, new_server_socket;
	
    //set socket's address information
    struct sockaddr_in   server_addr;
   
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons((uint16_t)(port));

    printf("app_tx_data_ch_main started \n");
    
Resocket:   
    // create a stream socket
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        printf("Create Data Socket Failed!\n");
        perror("socket");
        //close(server_socket);
        sleep(1);
        goto Resocket;
    }
    printf("Create Data Socket OK\n");

    //bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Data Bind Port: %d Failed!\n", port);
        perror("bind");
        close(server_socket);
        sleep(1);
        goto Resocket;
    }
    printf("Data bind socket ok ,port = %d \n",port);
    
	//sleep(5);
    
    // listen
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        printf("Server Data Listen Failed!\n");
        perror("listen");
        close(server_socket);
        sleep(1);
        goto Resocket;
    }
    printf("Data listen socket ok \n");
        
ReAccept:

	printf("Data Start Accept \n");
	new_server_socket = accept(server_socket, NULL,NULL);
	if (new_server_socket < 0)
	{
		printf("Server Data Accept Failed!\n");
		perror("accept");
		printf("accept errno : %d \n", errno);
        close(server_socket);
        sleep(1);
        goto Resocket;
	}
	else
	{
		printf("Server Data Aceept OK \n");
	}
    
    struct timeval tv1,tv2;
    struct timezone tz;
    //struct tcp_info info;
    
    //int optlen = sizeof(struct tcp_info);
    int pos, len;
    int iDataNum = 0;
    
    unsigned char*buf = NULL;
    
	iDataNum = 1000; //packet number
	
	buf = (unsigned char*)malloc(sizeof(unsigned char)*BUFFER_SIZE);
	
	long lDataRateSum = 0;
    int iDataRate = 0;
    int iTestTimes = 0;
    long iData[6] = {0};
    iData[0]=0;
    iData[1]=0;
    iData[2]=0;
    iData[3]=0;
    iData[4]=0;
    iData[5]=0;
    
    while(1)
    {		
        sleep(2);//Send every 2 seconds and also detect enable flag
        //printf("Detect data test every 2 seconds \n");
        //gbDataTestEn = SL_TRUE;
        //gbBandwidthDetectMode = SL_TRUE;
        if (SL_TRUE==gbDataTestEn || SL_TRUE==gbBandwidthDetectMode)
        {
            pos=0;
            len=0;
            iDataRate=0;
            iTestTimes += 1;
            //int i;
            
            printf("Start Sending Data Test \n");
            
	        gettimeofday(&tv1,&tz);
			
            while(pos < iDataNum)
            {
                //printf("pos < iDataNun \n");
                buf[0] = pos;
	            buf[BUFFER_SIZE-1] = 0;
						
				//send band data test
                len = send(new_server_socket, buf, BUFFER_SIZE,0);
                if(len <= 0)
                {
                    printf("Send data len <= 0 len= %d \n",len);
                    perror("send");
                    printf("send errno : %d \n", errno);
                  
                    close(new_server_socket);
                    goto ReAccept;
                }
                //printf("data send len = %d \n", len);
                
                pos++;
            }
            
            //last frame
			buf[0] = pos;
			buf[1] = 123;
			buf[250] = 123;
			buf[BUFFER_SIZE-1] = 123;
			len = send(new_server_socket, buf, BUFFER_SIZE,0);
			if(len <= 0)
			{
				perror("send");
				printf("send errno : %d \n", errno);
				close(new_server_socket);
				goto ReAccept;
			}
			printf("Send Last Data Rate Test Frame\n");
            //last frame

            gettimeofday(&tv2,&tz);
            printf("tv1 sec = %ld, usec=%ld \n",tv1.tv_sec,tv1.tv_usec);

            printf("tv2 sec = %ld, usec=%ld \n",tv2.tv_sec,tv2.tv_usec);

	        iData[0] = iData[1];
	        iData[1] = iData[2];
	        iData[2] = iData[3];
	        iData[3] = iData[4];
	        iData[4] = iData[5];
            iData[5] = iDataNum*1024*8/((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));

            lDataRateSum = iData[0]+iData[1]+iData[2]+iData[3]+iData[4]+iData[5];
            printf("Data Rate = %ld, Avg Rate = %ld \n",iData[5], lDataRateSum/6);
			
            /*
            len = recv(new_server_socket, "1", 1,0);
            if (len <= 0)
			{
				printf("data recv error \n");
				goto ReAccept; 
			}
			else
			{
				printf("data recv = %d \n", len);
			}
			*/
        }

        
    }
    //free(buffer);
    //free(myNode);
	free(buf);
    close(new_server_socket);
    printf("Send over!!!\n");
    return 0;
}

