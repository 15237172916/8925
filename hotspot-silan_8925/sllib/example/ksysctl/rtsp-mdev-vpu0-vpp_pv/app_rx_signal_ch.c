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

#include <sl_types.h>
#include <errno.h>
#include "app_rx_signal_ch.h"
#include "app_rx_io_ctl.h"

#define SIGNAL_CLIENT_PORT  8000

extern char serverip[128];
unsigned char signal_connect_state = 0; //0:not connect , 1: connect 

void *app_rx_signal_ch_main(void)
{
    //struct timeval tv1,tv2;
    //struct timezone tz;
    struct timeval timeout={2,0};
    //struct tcp_info info;
    int ret, len = 0;
    
    unsigned char buf[9];
    
    char * s = malloc(100*sizeof(char));
    
    //int optlen = sizeof(struct tcp_info);
    //int pos;
    //int iDataNum = 0;
    
    int sock_client;
    struct sockaddr_in servaddr;
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SIGNAL_CLIENT_PORT);
    servaddr.sin_addr.s_addr = inet_addr(serverip);
    
ReSocket:
	signal_connect_state = 0; //not connect
    sock_client = socket(AF_INET,SOCK_STREAM, 0);
	if (sock_client < 0)
    {
        printf("signal Create Socket Failed!\n");
        perror("socket");
        //close(sock_client);
        sleep(1);
        goto ReSocket;
    }
    
ReConnect:  

	//getsockopt(sock_cli,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *)&optlen);
	//printf("TCP Status = %d \n",info.tcpi_state);        
	signal_light_flash(); //light flash
    printf("--------------signal Connection start----------------------\n");
    ret = connect(sock_client, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        printf("signal channel connect error, ret = %d \n",ret);
        printf("errno is %d\n",errno);
        sleep(1);
        goto ReConnect;
    }
    //process_osd_disable();
    signal_connect_state = 1;
    //int nSndTimeOut;
    //timeout={1,0};
    
	setsockopt(sock_client,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));

	setsockopt(sock_client,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	//printf("Send Time Out = %d \n",timeout.tv_sec);    

	while(1)
	{
        sleep(1);
        //printf("g_RemoteCmd.bSendFlag = %d \n", g_RemoteCmd.bSendFlag);
		////////////////Recv Heart Shake/////////////////////////////////
		len = recv(sock_client, buf, 9, 0);
        if(len <= 0)
        {
            printf("Recv heart len < 0 \n");
            close(sock_client);
            sleep(1);
            goto ReSocket;
        }
        else
        {
			signal_connect_state = 1; //signal connect success
			//printf(" Recv heart len = %d buf[0]= %d\n",len,buf[0]);
        }
	
#if 1
		//heart mode 
		if (SL_FALSE == g_RemoteCmd.bSendFlag) 
		{
			//printf("Send heart signal \n");
			buf[0] = 8;
			buf[1] = 0;
			
			//send heart shake
			len = send(sock_client, buf, 9, 0);
			if (len <= 0)
			{
				printf("Send signal heart Error \n");
				perror("send");
				close(sock_client);
				sleep(1);
				goto ReSocket;
			}
			else
			{
				//printf("Send signal heart OK \n");
				//printf("send signal heart len = %d \n", len);
			}
			continue; //continue for next receive, no need to wait command ack
		}
#endif
#if 1
		///////////////Send Command to Tx///////////////////////////////   
        else //(SL_TRUE == g_RemoteCmd.bSendFlag)
        {
            g_RemoteCmd.bSendFlag = SL_FALSE;

            printf("Get Command to Send Cmdbuf[0]=%d Cmdbuf[1]=%d\n",g_RemoteCmd.uCmdBuf[0],g_RemoteCmd.uCmdBuf[1]);
            
            buf[0] = g_RemoteCmd.uCmdBuf[0]; //key value
            buf[1] = g_RemoteCmd.uCmdBuf[1]; //data test or change bitrate

            printf("Get Command to Send buf[0]=%d buf[1]=%d\n",buf[0],buf[1]);
	        len = send(sock_client, buf, 9,0);
            if(len <= 0)
            {
                printf("Send Error \n");
				close(sock_client);
				sleep(1);
				goto ReSocket;
            }
            
			//printf("Send OK \n");
	
	/***********************Get Ack***********************/ 
    
			printf("Wait for Ack back \n");
			len = recv(sock_client, buf, 9, 0);
			if (len <= 0)
            {
                printf("recv Error \n");
				close(sock_client);
				sleep(1);
				goto ReSocket;
            }
            else
            {
				printf("Get ACK buf[0] = %d \n", buf[0]);

				//Processing Ack(should make a seperate function) 2016-11-11
			    switch (buf[0])
			    {
			        case 1:
			        {
#if 1
	                    printf("****************buf[1] = %d ***************\n", buf[1]);
	                    if (buf[1] < 8)
							sprintf(s, "Get Ack - Video Level : Low");
						else if (buf[1] > 10)
							sprintf(s, "Get Ack - Video Level : High");
						else
							sprintf(s, "Get Ack - Video Level : Median");
#endif
	                    printf(s);
	                    g_ShowData.cText=s;
		                g_ShowData.bShowFlag = SL_TRUE;
						//g_RemoteCmd.bSendFlag = SL_FALSE; 
						g_ShowData.uShowTime = 2;                
		                break;
		            }
			        case 3:
			        {
	                    printf("Get Ack Date Test %d %d\n",buf[0],buf[1]);
	                    if(1 == buf[1])
	                    {
	                        g_ShowData.cText="Get Ack - Data Test On";
	                    }
	                    else if( 0 == buf[1])
	                    {
	                        g_ShowData.cText="Get Ack - Data Test Off";
	                    } 
		                g_ShowData.bShowFlag = SL_TRUE;
		                	                    
		                break;
		            }
		            case 8:
						printf("heard mode  \n");
		                break;

		            default:
		            {
		                sprintf(s, "Get Ack - Unknown Cmd Type = %d",buf[0]);	
                        g_ShowData.cText=s;
		                g_ShowData.bShowFlag = SL_TRUE;
		                break;	
		            }
		        }	    
            }
            len = 0;
        }
#endif        
/*******************Test OK Code*********************************
        len = recv(sock_cli, buf, 9, 0);
        if (len < 0)
        {
          //      printf("Server Recieve Data Failed!\n");
            printf(" Get len < 0 \n");
            break;
        }
        else if(len>0)
        {
			 printf("buf[0] = %d \n", buf[0]);
             
        }
	    if (len ==0)
		{
		    printf("get len = 0 \n");
    		continue;
		}
******************************************************************/		
	}
}
