#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <sys/mman.h>
#include <sl_types.h>
#include <sl_param_vcodec.h>
#include <pthread.h>
#include <linux/soundcard.h>
#include <errno.h>
#include "list_handler.h"
#include "audio_ioctl.h"
#include <sys/time.h>
#include "app_rtp_tx.h"
#include "ring_buffer.h"

#include "crc.h"
//#define RTP_UDP
//#define OUTPUT_H264
 
#define CHECKSUM_AUDIO
//#define CHECKSUM_VIDEO

extern volatile int viu_started; //main
extern volatile int viu_configed;
extern void *mutexlock;
extern LIST_BUFFER_S *list;

extern int flushMdev(void);
extern unsigned char HDMI_lost;

extern char web_flag;
extern char multicast[20];

DATAHEAD DataHead;

static FILE * outfile  = NULL;
static FILE * outfile1  = NULL;
char outfilename[128] = "./test1.264";
char outDisplay[20] = "/tmp/out.264";


static unsigned short csum(unsigned char *buf, int nwords)
{
	//unsigned long sum;
	unsigned short check;
	
	check = crc_check(buf, nwords);
	//usleep(1000);
	return (unsigned short)(~check);
}

#if 1
//UDP
SL_POINTER PullFromList(SL_POINTER p)
{
	void *buf;
	SL_S32 ret, len, pos , tmp_len;
	unsigned char *dst;
	unsigned char *pStream;
	unsigned char *pCheck="0abc"; //send flag with HDMI check signal
	SLVENC_ExtendStream_info_s *stream;

	dst = malloc(2*1024*1024); //FIXME
	if(!dst)
		printf("fail to malloc\n");
	//flush the list, get the newest data
	if (SL_NO_ERROR != SLOS_AcquireMutex(mutexlock,SL_INFINITE))
		return NULL;

	list_flush_data(list);
	if(viu_configed)
		//flushMdev(); //RTSP

	SLOS_ReleaseMutex(mutexlock);
#ifdef OUTPUT_H264
	if(!outfile)
	{   
		if ((outfile = fopen(outfilename, "wb")) == NULL) {
			printf("can't open %s",outfilename);
			return;
		}   
	}
#endif

#if 1
	//set socket's address information
	int sock_cli;
	socklen_t servlen_addr_length;
    struct sockaddr_in server_addr;
    servlen_addr_length = sizeof(server_addr);
    
ReSocket:
	web_flag = 0;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(RTP_PORT);
	//server_addr.sin_addr.s_addr=htonl(INADDR_BROADCAST); //UDP broadcast address 
	//server_addr.sin_addr.s_addr=inet_addr(MCAST_ADDR); //UDP multicast address
	server_addr.sin_addr.s_addr=inet_addr(multicast); //multicast 

    sock_cli = socket(AF_INET,SOCK_DGRAM, 0); //UDP
    if (sock_cli < 0)
    {
		perror("socket");
		sleep(1);
		goto ReSocket;
	}
#if 0
	const int opt = -1;
	setsockopt(sock_cli,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt));
#endif
	
#if 0
	//set Multicast loop
	int loop = 1; //1: on , 0: off
	ret = setsockopt(sock_cli,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop));
	if(ret < 0)
	{
		printf("set multicast loop error \n");
		perror("setsockopt");
	}
	
	//set multicast address 
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);       
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);     
	
	//add multicast group
	ret = setsockopt(sock_cli, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if(ret < 0)
	{
		printf("set multicast error \n");
		perror("setsockopt");
	}
#endif
#if 1
	int reuse = 1;
	ret = setsockopt(sock_cli, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
	if(ret < 0)
	{
		printf("set multicast reuse error \n");
		perror("setsockopt");
	}
#endif
#if 0
	char nochecksum=0;
	//setsockopt(sock_cli, IPPROTO, UDP_NO);

#endif
	struct timezone tz;
	struct timeval tv1, tv2;
	struct timeval timeout = {2,0};
	setsockopt(sock_cli,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
	setsockopt(sock_cli,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

#endif

	DataHead.iProbe = 0x1A1B1C1D;
    DataHead.uSeq = 0;
    DataHead.iTimeStamp = 0;
    
	//packet is UDP packet, frame is 264 frame;
	while (1) //continue get packet
	{
		//printf("web flag: %d \n", web_flag);
		if (1 == web_flag) //multicast switch 
		{
			sleep(1);
			close(sock_cli);
			goto ReSocket;
		}
		//check HDMi signal 
		if (HDMI_lost)
		{
			len = sendto(sock_cli, pCheck, sizeof(pCheck), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
			if (len <= 0)
			{
				perror("sendto");
				close(sock_cli);
				goto ReSocket;
			}
			sleep(1);
		}
		else
		{

		/***************Video data****************/
#if 1
		//ret = list_fetch_data(list, &buf);
		if (list_fetch_data(list, &buf)) //
		{
			//printf("list fetch failed \n");
			usleep(1000);
			continue;
		}
		else
		{
			#if 1
			ret = gettimeofday(&tv1,&tz);
			if (ret < 0){
				printf("printf_log gettimeofday error \n");
				perror("gettimeofday");
			}
			#endif
			
			DataHead.uPayloadType = H264;
			DataHead.uSeq += 1;
			DataHead.iTimeStamp = tv1.tv_sec-144221050;
			//printf("tv1.tv_sec : %ld \n", DataHead.iTimeStamp);
			//printf("tv1.tv_sec : %ld \n", tv1.tv_usec);
			//printf("uSeq : %d \n", DataHead.uSeq);
			
			//printf("\n-------------------------------------------------\n");
			
			stream = (SLVENC_ExtendStream_info_s *)buf;
			if (stream->header_size) //Idr Frame
			{
	#if 1
				//printf("this is I frame \n");
				//printf("stream->header_size : %d \n", stream->header_size);
				//printf("stream->stream_size : %d \n", stream->stream_size);
				
				DataHead.iLen =  stream->header_size + stream->stream_size; //NAL len
				
				tmp_len = stream->stream_size + stream->header_size;
				memcpy(dst, (unsigned char *)stream + stream->header_offset_addr, stream->header_size);
				memcpy(dst + stream->header_size, (unsigned char *)stream + stream->stream_offset_addr, stream->stream_size);
				pStream = (unsigned char *)dst;
				//printf("I frame, video data lenght: %d \n", DataHead.iLen);
#ifdef CHECKSUM_VIDEO
				if (DataHead.iLen < CHECK_SUM_COUNT)
				{
					DataHead.usChecksum = csum((unsigned char *)pStream, DataHead.iLen); //check sum
				}
				else
				{
					DataHead.usChecksum = csum((unsigned char *)pStream, CHECK_SUM_COUNT); 
				}
#endif
				//printf("DataHead.usChecksum: %x \n", DataHead.usChecksum);
				//printf("*pStream: %x \n", *pStream);
				#if 0
				int m, n;
				unsigned char * p = stream;
				for (m=256; m>0; m--)
				{
					for (n=40; n>0; n--)
					{
						printf("%x",*p);
						p+=1;
					}
					printf("\n");
				}
				#endif
				
				//send Data Header
				len = sendto(sock_cli, &DataHead, sizeof(DataHead), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
				if(len <= 0)
				{
					printf("Send data len <= 0 len= %d \n",len);
					printf("errno is %d\n",errno);
					perror("sendto");
					//close(sock_cli);
					//goto ReSocket;
				}
				//printf("send len : %d \n", len);
				//printf("stream->header_size : %d \n", stream->header_size);
				
				//send I frame data header
				while (tmp_len > UDP_MTU)
				{
					//printf("dst : %d \n", dst);
					//printf("pStream : %d \n", pStream);
					//printf("stream->stream_size : %d \n", stream->stream_size);
					//printf("tmp_len : %d \n", tmp_len);
					
					//send Data Body
					len = sendto(sock_cli, pStream, UDP_MTU, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
					if(len <= 0)
					{
						printf("Send data len <= 0 len= %d \n",len);
						printf("errno is %d\n",errno);
						//close(sock_cli);
						//goto ReSocket;
					}
					
					//printf("send h264 I frame len = %d \n",len);
	#ifdef OUTPUT_H264
					fwrite((unsigned char *)pStream, UDP_MTU, 1, outfile);
	#endif
					tmp_len -= UDP_MTU;
					pStream += UDP_MTU;
				}
				len = sendto(sock_cli, pStream, tmp_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
				if(len <= 0)
				{
					printf("Send data len <= 0 len= %d \n",len);
					printf("errno is %d\n",errno);
					//close(sock_cli);
					//goto ReSocket;
				}
	#ifdef OUTPUT_H264
				fwrite((unsigned char *)pStream, tmp_len, 1, outfile);
				//fclose(outfile);
	#endif
				//printf("send h264 I frame len = %d \n",len);
	#endif
			}
			else
			{
				DataHead.iLen =  stream->stream_size;
				
				tmp_len = stream->stream_size;
				pStream = (unsigned char *)stream + stream->stream_offset_addr;//dst;
				//printf("P frame, video data lenght: %d \n", DataHead.iLen);
#ifdef CHECKSUM_VIDEO
				if (DataHead.iLen < CHECK_SUM_COUNT)
				{
					DataHead.usChecksum = csum((unsigned char *)pStream, DataHead.iLen); //check sum
				}
				else
				{
					DataHead.usChecksum = csum((unsigned char *)pStream, CHECK_SUM_COUNT);
				}
#endif

				//printf("video data lenght: %d \n", DataHead.iLen);
				//printf("DataHead.usChecksum: %x \n", DataHead.usChecksum);
				//printf("*stream: %x \n", *pStream);
				
				
				//send data header
				len = sendto(sock_cli, &DataHead, sizeof(DataHead), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
				if(len <= 0)
				{
					printf("Send data len <= 0 len= %d \n",len);
					printf("errno is %d\n",errno);
					//close(sock_cli);
					//goto ReSocket;
				}
				
				//printf("Send data len = %d \n",len);
				
				//send data body(264 NAL)
				while (tmp_len > UDP_MTU)
				{
					len = sendto(sock_cli, pStream, UDP_MTU, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
					if (len <= 0)
					{
						printf("Send data len <= 0 len= %d \n",len);
						printf("errno is %d\n",errno);
						//close(sock_cli);
						//goto ReSocket;
					}
	#ifdef OUTPUT_H264
					//fwrite((unsigned char *)pStream, UDP_MTU, 1, outfile);
	#endif
					//printf("send h264 P frame len = %d \n",len);
					
					tmp_len -= UDP_MTU;
					pStream += UDP_MTU;
				}
				len = sendto(sock_cli, pStream, tmp_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
				if (len <= 0)
				{
					printf("Send data len <= 0 len= %d \n",len);
					printf("errno is %d\n",errno);
					//close(sock_cli);
					//goto ReSocket;
				}
	#ifdef OUTPUT_H264
				fwrite((unsigned char *)pStream, tmp_len, 1, outfile);
	#endif
				//printf("send h264 P frame len = %d \n",len);
				//printf("Get frame without header, size is %d",stream->stream_size);
			}
		}

#endif

		/***************Audio data****************/	
#if 1
		if (RingGetByteStreamMemoryBufferCnt() > FETCH_COUNT)
		{
			ret = gettimeofday(&tv1,&tz);
			if (ret < 0){
				printf("printf_log gettimeofday error \n");
				perror("gettimeofday");
			}
			DataHead.uPayloadType = WAV;
			DataHead.iTimeStamp = tv1.tv_sec-144221050;
			DataHead.iLen = FETCH_COUNT;
			
			//printf("\n*****************************************************\n");
			
			RingPullFromByteStreamMemoryBuffer(buf, FETCH_COUNT);
			tmp_len = FETCH_COUNT;
			pStream = (unsigned char *)buf;//(unsigned char *)dst;
			//printf("pStream: %d \n", *pStream);
#ifdef CHECKSUM_AUDIO
			DataHead.usChecksum = csum(pStream, FETCH_COUNT);
#endif
			//printf("usChecksum: %x \n", DataHead.usChecksum);
			#if 0
			int m, n;
			unsigned char * p = pStream;
			for (m=256; m>0; m--)
			{
				for (n=40; n>0; n--)
				{
					printf("%x",*p);
					p+=1;
				}
				printf("\n");
			}
			#endif
			
			//send Data Header
			len = sendto(sock_cli, &DataHead, sizeof(DataHead), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				//close(sock_cli);
				//goto ReSocket;
			}
			
			//loop send audio data
			while (tmp_len > UDP_MTU)
			{
				len = sendto(sock_cli, pStream, UDP_MTU, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
				if(len <= 0)
				{
					printf("Send data len <= 0 len= %d \n",len);
					printf("errno is %d\n",errno);
					//close(sock_cli);
					//goto ReSocket;
				}
				//printf("send packet len : %d \n", len);
				//fwrite(buffer, buffer_size, 1, outfile);
				tmp_len -= UDP_MTU;
				pStream += UDP_MTU;
			}
			len = sendto(sock_cli, pStream, tmp_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				//close(sock_cli);
				//goto ReSocket;
			}
		}
		else
		{
			//printf("ringbuffer not full \n");
			usleep(1000);
		}
		}
#endif
	}

    close(sock_cli);
	free(dst);
	
    return 0;
}
 
#else
//TCP
SL_POINTER  PullFromList(SL_POINTER p)
{
	void *buf;
	SL_S32 ret, len, pos;
	unsigned char *dst;
	const int opt = -1;
	
	SLVENC_ExtendStream_info_s *stream;

	dst = malloc(2*1024*1024); //FIXME
	if(!dst)
		printf("fail to malloc\n");
	//flush the list, get the newest data
	if (SL_NO_ERROR != SLOS_AcquireMutex(mutexlock,SL_INFINITE))
		return NULL;

	list_flush_data(list);
	if(viu_configed)
		//flushMdev(); //RTSP

	SLOS_ReleaseMutex(mutexlock);
	
	
	if(!outfile)
	{   
		if ((outfile = fopen(outfilename, "wb")) == NULL) {
			printf("can't open %s",outfilename);
			return;
		}   
	}

	//int server_socket, new_server_socket;
	int rtp_server_socket = 0, rtp_new_server_socket = 0;
	socklen_t servlen_addr_length;
	struct sockaddr_in   server_addr;
	servlen_addr_length = sizeof(server_addr);
	struct tcp_info info;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port = htons(8001);
	
ReSocket:
	// create a stream socket

	//rtp_server_socket = socket(AF_INET, SOCK_DGRAM, 0); //UDP
	rtp_server_socket = socket(AF_INET, SOCK_STREAM, 0); //TCP
	if (rtp_server_socket < 0)
	{
		printf("Create Socket Failed!\n");
		perror("socket");
		goto ReSocket;
	}
	
    printf("Create Socket OK \n");
    //setsockopt(rtp_server_socket,SOL_SOCKET,SO_REUSEADDR,&info,sizeof(info));
    //bind
    if (bind(rtp_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Bind Port: 8001 Failed!\n");
        perror("bind");
        close(rtp_server_socket);
        goto ReSocket;
    }
    printf("bind socket ok \n");
	//sleep(5);
    //listen
    if (listen(rtp_server_socket, 20))
    {
        printf("Server Listen Failed!\n");
        perror("listen");
        close(rtp_server_socket);
        goto ReSocket;
    }
    printf("listen socket ok \n");
        
ReAccept:

	printf("Start Accept \n");
	rtp_new_server_socket = accept(rtp_server_socket, NULL,NULL);
	printf("Finish Accept \n");
	if (rtp_new_server_socket < 0)
	{
		printf("Server Accept Failed!\n");
		perror("accept");
		goto ReAccept;
	}
	printf("Server Aceept OK \n");

    
    struct timeval tv1, tv2, timeout = {2,0};
    struct timezone tz;
    
    DataHead.iProbe = 0x1A1B1C1D;
    DataHead.uSeq = 0;
    DataHead.iTimeStamp = 0;


	setsockopt(rtp_new_server_socket,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
	setsockopt(rtp_new_server_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	//sleep(2);

	while (1)
	{

		/***************Video data****************/
#if 1
		ret = list_fetch_data(list, &buf);
		if(ret)
		{
			usleep(5000);
			continue;
		}
		ret = gettimeofday(&tv1,&tz);
		if (ret < 0){
			printf("printf_log gettimeofday error \n");
			perror("gettimeofday");
		}
		
		DataHead.uPayloadType = H264;
        DataHead.uSeq += 1;
        DataHead.iTimeStamp = tv1.tv_sec-144221050;
        //printf("tv1.tv_sec : %ld \n", DataHead.iTimeStamp);
        //printf("tv1.tv_sec : %ld \n", tv1.tv_usec);
		//printf("uSeq : %d \n", DataHead.uSeq);
		
		printf("\n-------------------------------------------------\n");
		
		stream = (SLVENC_ExtendStream_info_s *)buf;
		//printf("stream->header_size : %d \n", stream->header_size);
		if(stream->header_size) //Idr Frame
		{
			if(stream->header_size > 23)
			{
				//reboot1();//FIXME
			}
			
			DataHead.iLen =  stream->header_size + stream->stream_size; //NAL len
			
			//send Data Header

			len = send(rtp_new_server_socket, &DataHead, sizeof(DataHead),0);
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(rtp_new_server_socket);
				goto ReAccept;
			}
			
			//printf("dst : %d \n", dst);
			memcpy(dst, (unsigned char *)stream + stream->header_offset_addr, stream->header_size);
			memcpy(dst + stream->header_size, (unsigned char *)stream + stream->stream_offset_addr, stream->stream_size);
			
			int i,templen;
			templen = stream->header_size+stream->stream_size;
			
	        //send Data Body

			len = send(rtp_new_server_socket, dst, stream->header_size + stream->stream_size,0);
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(rtp_new_server_socket);
				goto ReAccept;
			}
			printf("send h264 I frame len = %d \n",len);
			
	        //printf("Get frame with header, size is %d",stream->header_size + stream->stream_size);
		}
		else //P Frame
		{
            DataHead.iLen =  stream->stream_size;
            
            //send data header

			len = send(rtp_new_server_socket, &DataHead, sizeof(DataHead),0);
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(rtp_new_server_socket);
				goto ReAccept;
			}
			
			 //printf("Send data len = %d \n",len);
			
			//send data body(264 NAL)

			len = send(rtp_new_server_socket, (unsigned char *)stream + stream->stream_offset_addr, stream->stream_size,0);
			if (len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(rtp_new_server_socket);
				goto ReAccept;
			}
			printf("send h264 P frame len = %d \n",len);
			
			//printf("Get frame without header, size is %d",stream->stream_size);
		}
#endif
		
		/***************Audio data****************/	
#if 1
		if (RingGetByteStreamMemoryBufferCnt() > FETCH_COUNT)
		{
			ret = gettimeofday(&tv1,&tz);
			if (ret < 0){
				printf("printf_log gettimeofday error \n");
				perror("gettimeofday");
			}
			DataHead.uPayloadType = WAV;
			DataHead.iTimeStamp = tv1.tv_sec-144221050;
			DataHead.iLen = FETCH_COUNT;
			
			printf("\n*****************************************************\n");
			
			//send Data Header
			len = send(rtp_new_server_socket, &DataHead, sizeof(DataHead),0);
			if(len <= 0)
			{
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(rtp_new_server_socket);
				goto ReAccept;
			}
			RingPullFromByteStreamMemoryBuffer(buf, FETCH_COUNT);
			
			len = send(rtp_new_server_socket, buffer, FETCH_COUNT, 0);
			if(len <= 0)
			{
				send_over = 1;
				printf("Send data len <= 0 len= %d \n",len);
				printf("errno is %d\n",errno);
				close(rtp_new_server_socket);
				goto ReAccept;
			}
		}
		
#endif
	}
	free(dst);

	return NULL;
}
 
#endif

