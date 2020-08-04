#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include "audio_ioctl.h"
#include <sys/mman.h>

static audio_dma_info_s audio_dma_out;
static int audio_fd = -1;
static int audioOut_trigger = 0;

extern SL_S32 audio_change;
extern char idr_flag;
extern SL_U32 g_fs;
extern SL_U32 g_audio_bits;
extern SL_U32 g_chns;

void audio_close(void)
{
	if (audio_fd > 0)
	{
		close(audio_fd);
		audio_fd = -1;
		
		printf("\n\n ***audio close \n\n");
	}
}

void * check_wr_thread(void * Args)
{
	static unsigned int wr_old = 0;
	unsigned int tmp;

	while(1)
	{
		if(audioOut_trigger > 0)
		{
			if (audio_fd < 0)
			{
				audio_config(g_fs, g_audio_bits, g_chns);
			}	
			if(wr_old == audio_dma_out.wrPtr)
			{
				if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER_OFF, &tmp) == -1) {
					audio_close();
				}
				memset((unsigned char *)(audio_dma_out.start_addr_va), 0x00, audio_dma_out.size);
				audioOut_trigger = 0;
				printf("\n\n*** 1 audioOut_trigger_off\n\n");
				audio_dma_out.wrPtr = audio_dma_out.rdPtr = audio_dma_out.start_addr;
				wr_old = 0;
			}
			else 
				wr_old = audio_dma_out.wrPtr;
		}
		usleep(200000);
	}

	return NULL;
}

int audio_config(SL_U32 fs, SL_U32 audio_bits, SL_U32 chns)
{
	unsigned int tmp;
	
	audio_close();
	
	printf("audio_fd : %d \n", audio_fd);
	if (audio_fd < 0)
	{
		audio_fd=open("/dev/silan-i2s", O_RDWR);
		if (audio_fd < 0)
		{
			printf("fail to open /dev/silan-i2s\n");
			return -1;
		}
		if (ioctl(audio_fd, AUDIO_IOCTL_GET_OUT_START_DMA_ADDR, &tmp) == -1) {
			printf("AUDIO_IOCTL_GET_OUT_START_DMA_ADDR error \n");
			audio_close();
			return -1;
		}
		audio_dma_out.start_addr = tmp;
		audio_dma_out.wrPtr = audio_dma_out.rdPtr = audio_dma_out.start_addr;

		if (ioctl(audio_fd, AUDIO_IOCTL_GET_OUT_DMA_SIZE, &tmp) == -1) {
			printf("AUDIO_IOCTL_GET_OUT_DMA_SIZE error \n");
			audio_close();
			return -1;
		}
		audio_dma_out.size = tmp;

		tmp =  (unsigned long)mmap(NULL, audio_dma_out.size, PROT_READ | PROT_WRITE, MAP_SHARED, audio_fd, 0);

		audio_dma_out.start_addr_va = tmp;

	}
	printf("audio_fd : %d \n", audio_fd);
	printf("audioOut_trigger : %d \n", audioOut_trigger);
#if 1
	if(audioOut_trigger) 
	{
		//trigger
		if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER_OFF, &tmp) == -1) {
			printf("AUDIO_IOCTL_OUT_TRIGGER_OFF error \n");
			audio_close();
			return -1;
		}
		memset((unsigned char *)(audio_dma_out.start_addr_va), 0x00, audio_dma_out.size);
		audioOut_trigger = 0;
		printf("\n\n*** 2 audioOut_trigger_off\n\n");
	}
#endif
	audio_dma_out.wrPtr = audio_dma_out.rdPtr = audio_dma_out.start_addr;

	tmp = chns; //2;//FIXME
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_OUT_CHNS, &tmp) == -1) {
		printf("AUDIO_IOCTL_SET_OUT_CHNS error \n");
		audio_close();
		return -1;
	}

	tmp = audio_bits;//16; //wxp
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_OUT_BITS, &tmp) == -1) {
		printf("AUDIO_IOCTL_SET_OUT_BITS error \n");
		audio_close();
		return -1;
	}
	tmp = fs;//48000; //wxp
	if (ioctl(audio_fd, AUDIO_IOCTL_SET_OUT_FS, &tmp) == -1) {
		printf("AUDIO_IOCTL_SET_OUT_FS error \n");
		audio_close();
		return -1;
	}
#if 0
	tmp = NULL;
	if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER, &tmp))
	{
		printf("AUDIO_IOCTL_OUT_TRIGGER error \n");
		audio_close();
		return ;
	}
	printf("*** 1 audioOut_trigger_on\n");
#endif
	printf("fs : %d , bits : %d , chns : %d \n", fs, audio_bits, chns);

	if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER_OFF, &tmp) == -1) {
		audio_close();
	}
	
	return 0;
}

void audio_play(unsigned char * src, unsigned frameSize)
{
	unsigned char *dst;
	int write_size_1;
	int write_size_2;
	unsigned int tmp;
	#if 1
	if (idr_flag != 0)
	{
		//printf("*");
		audio_close();
		return 0;
	}
	else
	{
		#if 0
		if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER, &tmp))
		{
			printf("AUDIO_IOCTL_OUT_TRIGGER error \n");
			audio_close();
			return ;
		}
		#endif
		//printf("audio frame size : %d \n", frameSize);
	}
	#endif
	
	//printf("//////////////////Frame size : %d /////////////\n", frameSize);
#if 0
#define DUMP_AUDIO_DATA
#ifdef DUMP_AUDIO_DATA
#define DUMP_COUNT_TOTAL 50000
	static int dump_count = 0;
	char outfilename[128] = "./test3.wav";
	static FILE * outfile  = NULL;
	if(!outfile)
	{   
		if ((outfile = fopen(outfilename, "wb")) == NULL) {
			printf("can't open %s",outfilename);
			return;
		}   
	}
	if (outfile)
	{   
		if(DUMP_COUNT_TOTAL>dump_count)
			fwrite(src, frameSize, 1, outfile);
		dump_count ++;
		if(DUMP_COUNT_TOTAL == dump_count)
		{
			fclose(outfile);
		}
	}
#endif
#endif
	if (audio_fd < 0)
	{
		audio_config(g_fs, g_audio_bits, g_chns);
	}	
	if((audio_dma_out.wrPtr + frameSize) < (audio_dma_out.start_addr + audio_dma_out.size))
	{
		dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);
		memcpy(dst, src, frameSize);
		audio_dma_out.wrPtr += frameSize;
		//printf("audio_dma_out.wrPtr = %d \n", audio_dma_out.wrPtr);
	}
	else
	{
		write_size_1 = audio_dma_out.start_addr + audio_dma_out.size - audio_dma_out.wrPtr;

		dst =(unsigned char *)(audio_dma_out.start_addr_va + audio_dma_out.wrPtr - audio_dma_out.start_addr);

		memcpy(dst, src, write_size_1);

		write_size_2 = frameSize - write_size_1;

		dst =(unsigned char *)audio_dma_out.start_addr_va;
		src += write_size_1;
		memcpy(dst, src, write_size_2);

		audio_dma_out.wrPtr = audio_dma_out.start_addr + write_size_2;
	}
	//printf("audio_dma_out.wrPtr = %d \n", audio_dma_out.wrPtr);
	
	if(!audioOut_trigger) 
	{
		tmp = audio_dma_out.wrPtr - audio_dma_out.start_addr;
		//printf("tmp = %d \n", tmp);
		if(tmp > 60*1024) 
		{
			if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER, &tmp))
			{
				printf("AUDIO_IOCTL_OUT_TRIGGER error \n");
				audio_close();
				return ;
			}
			printf("*** audioOut_trigger_on\n");
			audioOut_trigger = 1;
			//audio_change = 1;
		}
	}
	//printf("audioOut tigger : %d \n", audioOut_trigger);
	if(audioOut_trigger) 
	{
		if (ioctl(audio_fd, AUDIO_IOCTL_GET_OUT_RD_DMA_ADDR, &tmp) == -1) {
			printf("AUDIO_IOCTL_GET_OUT_RD_DMA_ADDR error \n");
			audio_close();
			return;
		}

		if(audio_dma_out.wrPtr >= tmp)
		{
			if((audio_dma_out.wrPtr - tmp) < 4000)
			{
				if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER_OFF, &tmp) == -1) {
					printf("AUDIO_IOCTL_OUT_TRIGGER_OFF error \n");
					audio_close();
				}
				memset((unsigned char *)(audio_dma_out.start_addr_va), 0x00, audio_dma_out.size);
				audioOut_trigger = 0;
				printf("\n\n*** 4 audioOut_trigger_off\n\n");
				audio_dma_out.wrPtr = audio_dma_out.rdPtr = audio_dma_out.start_addr;
				audioOut_trigger = 0;
			}
		}
		else
		{
			if((tmp - audio_dma_out.wrPtr) > audio_dma_out.size - 4000)
			{
				if (ioctl(audio_fd, AUDIO_IOCTL_OUT_TRIGGER_OFF, &tmp) == -1) {
					printf("AUDIO_IOCTL_OUT_TRIGGER_OFF error \n");
					audio_close();
				}
				memset((unsigned char *)(audio_dma_out.start_addr_va), 0x00, audio_dma_out.size);
				audioOut_trigger = 0;
				printf("\n\n*** 5 audioOut_trigger_off\n\n");
				audio_dma_out.wrPtr = audio_dma_out.rdPtr = audio_dma_out.start_addr;
				audioOut_trigger = 0;
			}
		}
	}
}


