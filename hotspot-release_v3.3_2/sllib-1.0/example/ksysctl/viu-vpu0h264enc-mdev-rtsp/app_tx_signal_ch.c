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
#include <errno.h>

#include "app_tx_signal_ch.h"

//#define DEBUG_LOG

#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024
#define WIFI_TIME 1


extern int reboot1(void);

void *app_tx_signal_ch_main(void * arg)
{
	int len = 0;
    int port = *((int *)arg);
    int server_socket = 0;
    int new_server_socket = 0;
   
    unsigned char buf[9] = {0};
    
    // set socket's address information
    struct sockaddr_in server_addr;
    //socklen_t length = sizeof(server_addr);
    
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
ReSocket:
	printf("signal socket started \n");
    // create a stream socket
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        printf("signal Create Socket Failed!\n");
        perror("socket");
        
        sleep(1);
        goto ReSocket;
    }
    printf("Create signal Socket OK \n");
	
    //bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server signal Bind Port: %d Failed!\n", port);
        perror("bind");
        close(server_socket);
        sleep(1);
        goto ReSocket;
    }
   
    //sleep(5);
    
    // listen
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        printf("Server signal Listen Failed!\n");
        perror("listen");
        close(server_socket);
        sleep(1);
        goto ReSocket;
    }

ReAccept:
	// accept
    printf("signal Start Accept \n");
    new_server_socket = accept(server_socket, NULL,NULL);
    printf("new_server_socket = %d \n", new_server_socket);
    if (new_server_socket <= 0)
    {
        printf("Server signal Accept Failed!\n");
        perror("accept");
        close(server_socket);
        sleep(1);
        goto ReSocket;	
    }
	printf("Server signal Accept success !\n");
       
    struct timeval timeout={2,0};
    
    setsockopt(new_server_socket,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
    setsockopt(new_server_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    //printf("Send Time Out = %d \n",timeout.tv_sec);  

    while (1)
    {
        buf[0] = 8; //heart shake
        
        /*
        *   send heart shake 
        */
        len = send(new_server_socket, buf,9,0);
        if (len <= 0)
        {
            printf("Send heart len = %d \n", len);
            perror("send");
            printf("send errno : %d \n", errno);
            
            close(new_server_socket);
            goto ReAccept;
        }
        if(len > 0)
        {
            //printf("Send heart len = %d\n",len);
        }
        /*
        *   recv heart shake
        */
        sleep(1);
#if 1
        len = recv(new_server_socket, buf, 9, 0);
        if (len <= 0)
        {
            printf("Server Recieve signal Failed!\n");
            perror("recv");
            printf("signal recv len = %d \n", len);
            printf("recv errno : %d \n", errno);
  
            close(new_server_socket);
            goto ReAccept;
        }
        else
        {
			//printf("buf[0] = %d \n", buf[0]);
			//printf("recv len = %d \n", len);
#if 1
			g_TxCommand.bCmdFlag = SL_TRUE;
			//g_TxCommand.iCmdType = buf[0];
			g_TxCommand.iCmdData[0] = buf[0];
			g_TxCommand.iCmdData[1] = buf[1];
			
            //printf("Setting Command processing \n");     
#if 1
			/*
			 * Send Ack back
			 */
            sleep(1);
            
            if (SL_TRUE == g_RemoteCmd.bSendFlag)
            {
                //printf("Process RemoteCommand \n");
                
                g_RemoteCmd.bSendFlag = SL_FALSE;
            
                buf[0] = g_RemoteCmd.uCmdBuf[0];
                buf[1] = g_RemoteCmd.uCmdBuf[1];

	            len=send(new_server_socket, buf, 9,0);
                if (len <= 0)
                {
                    printf("Send signal data Error \n");
                    perror("send");
                    printf("send errno : %d \n", errno);
                    close(new_server_socket);
					goto ReAccept;
                }
                else
                {
                    //printf("Send OK \n");
                }
            }            
#endif
#endif
        }
#endif
   
    }
	free(buf);
    close(new_server_socket);
    printf("Send over!!!\n");
    return 0;
}
