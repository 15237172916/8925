#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#include "app_rx_io_ctl.h"
#include "app_rx_data_ch.h"

#define MYPORT    8001
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024
#define DATANUM 10000000

extern SL_S32 g_discard_frame;
extern unsigned char signal_connect_state;

typedef struct
{
//    int ab;
    int num[DATANUM];
}Node;

extern char serverip[128];

void *app_rx_data_ch_main(void)
{
    struct timeval tv1,tv2, timeout = {10,0};
    struct timezone tz;

	//unsigned char buf[9];
    
    int iDataNum = 0;

    int sock_cli = 0;
    
    gbTestMode == SL_FALSE;
    gbBandwidthDetectMode == SL_FALSE;
    
	//set socket's address information
	struct sockaddr_in clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(MYPORT);
    clientaddr.sin_addr.s_addr = inet_addr(serverip);

ReSocket:
    sock_cli = socket(AF_INET,SOCK_STREAM, 0);
	if (sock_cli < 0)
	{
		printf("RX data channel Create Socket Failed!\n");
		perror("socket");
		
		sleep(1);
		goto ReSocket;
	}
	
    sleep(3);
    
ReConnect:
	printf("signal_connect_state = %d \n", signal_connect_state);
	
	if (1 == signal_connect_state){
		
    printf("----------------RX Data Channel Connection start----------------------\n");
    
    if (connect(sock_cli, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0)
    {
        printf("8001 data channel connect error \n");
        perror(connect);
        
        sleep(1);
        goto ReConnect;
    }
    printf("8001 data channel Connected \n");
	

	setsockopt(sock_cli,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));

	setsockopt(sock_cli,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));


	unsigned char *buf = (unsigned char*)malloc(sizeof(unsigned char)*BUFFER_SIZE);


    long lDataRateSum=0;
    int iDataRate;
    int iTestTimes=0;
    long iData[6];
    iData[0]=0;
    iData[1]=0;
    iData[2]=0;
    iData[3]=0;
    iData[4]=0;
    iData[5]=0;
    
    char *s = malloc(100*sizeof(char));
	sleep(10);
	
    while(1)                                                                                                               
    {
        int pos=0;
        int len;
        int bStartRecv=0;
        int i;
        
        //gbTestMode = SL_TRUE;
        //gbBandwidthDetectMode = SL_TRUE;
        //printf("gbTestMode = %d , gbBandwidthDetectMode = %d \n", gbTestMode, gbBandwidthDetectMode);
        if( SL_TRUE == gbTestMode || SL_TRUE == gbBandwidthDetectMode)
        //if (g_server_connect)
        {
            while(1) //continue get frame
            {                                                                                                            
                len = recv(sock_cli, buf, BUFFER_SIZE, 0);
                if (len <= 0)
                {
                    printf("Server Recieve Data Failed!\n");
					perror("recv");
					printf("data recv len = %d \n", len);
					//printf("recv errno : %d \n", errno);
					
                    close(sock_cli);
                    goto ReSocket;
                }
                if(len>0 && bStartRecv<1)
                {
			        gettimeofday(&tv1,&tz); //first time get frame
                    bStartRecv = 1;
                }
	            
                pos++;
                
                if(buf[len-1]==123) //get last frame
	            {
                    buf[0]=0;
		            break;
	            }
            }
            if (len >0) //valid data
            {
                gettimeofday(&tv2,&tz);
        
                printf("get %d frame\n", pos);
                printf("tv1 sec = %d, usec=%d \n",tv1.tv_sec,tv1.tv_usec);

                printf("tv2 sec = %d, usec=%d \n",tv2.tv_sec,tv2.tv_usec);

	            iData[0] = iData[1];
	            iData[1] = iData[2];
	            iData[2] = iData[3];
	            iData[3] = iData[4];
	            iData[4] = iData[5];
                iData[5] = pos*1024*8/((tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));

                lDataRateSum = iData[0]+iData[1]+iData[2]+iData[3]+iData[4]+iData[5];
        
                printf("Data Rate = %d, Avg Rate = %d \n",iData[5], lDataRateSum/6);
                	    
        	    sprintf(s, "Data Rate = %d, Avg Rate = %d \n",iData[5], lDataRateSum/6);	
                printf(s);
                if( SL_TRUE == gbTestMode )
                {
                    g_ShowData.cText=s;
                    g_ShowData.uShowTime = 0; //Keep Show
                    g_ShowData.bShowFlag = SL_TRUE;
                }
                
                if(iData[5]>30)  //High Level
                {
                    signal_light_high_level();
                }
                else if(iData[5]>20) //Mid Level
                {
                    signal_light_mid_level();
                }
                else//Low Level
                {
                    signal_light_low_level();
                }
            }
            usleep(1000); //Detect gbTestMode change to disable every 1ms;
            
        }
        else //gbTestMode == SL_FALSE && gbBandwidthDetectMode == SL_FALSE
        {
            sleep(2); // Detect gbTestMode change every 2 seconds; Process g_discard_frame
            //if(0 < g_discard_frame )
            //{
                printf("g_discard_frame = %d \n",g_discard_frame);  
            //}
            
            
			if(g_discard_frame < 3)  //High Level
			{
				signal_light_high_level();
			}
			else if(g_discard_frame < 10) //Mid Level
			{
				signal_light_mid_level();
			}
			else//Low Level
			{
				signal_light_low_level();
			}
        } 
        
    }
	}
	else
	{
		sleep(1);
		goto ReConnect;
	}
    close(sock_cli);
	//free(buf);

    return 0;
}

