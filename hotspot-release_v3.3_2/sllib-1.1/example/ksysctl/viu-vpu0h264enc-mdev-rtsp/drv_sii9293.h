#ifndef __DRV_SII9293_H__
#define __DRV_SII9293_H__

#define AUDIO_OUTPUT
typedef unsigned int UINT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;

typedef enum 
{
	AudioOff,
	AudioReq,
	AudioConfirm,
	AudioOn
	
} audio_status_e;

typedef enum 
{
	Unplug,
	WaitSync,
	CheckSync,
	ModeDetect,
	VideoOn,
	HDCP_Reset,
	SWReset
	
}video_status_e;

typedef enum 
{
	RGB24,
	YUV422,
	YUV444
	
} hdmi_mode_e;

typedef struct _video_info
{
	UINT32 hdmiMode;
	UINT32 width;
	UINT32 height;
	UINT32 frameRate;
	UINT32 avmute;
	UINT32 input_vsync_polarity; 
	UINT32 input_hsync_polarity; 
	UINT32 interlaceMode;
	UINT32 vtavl;
	UINT32 vfp;
	hdmi_mode_e hdmi_mode;

} video_info_s;

typedef struct _audio_info
{
	UINT32 fs;
	UINT32 audio_bits;
	UINT32 chns;
} audio_info_s;

typedef struct _sii9293_chip
{
	audio_status_e audio_status;
	video_status_e video_status;
	video_info_s video_info;
	audio_info_s audio_info;
	UINT32 g_video_output;
	UINT32 g_audio_output;
	UINT32 video_prev_status;
	UINT32 audio_prev_status;
	
} sii9293_chip_s;

typedef struct _audio_dma_info
{
	UINT32 start_addr;
	UINT32 start_addr_va;
	UINT32 size;
	UINT32 wrPtr;
	UINT32 rdPtr;
	
} audio_dma_info_s;

typedef struct {
	UINT8 AudioFlag ;
	UINT8 AudSrcEnable ;
	UINT8 SampleFreq ;
	UINT8 ChStat[5] ;
	
} AUDIO_CAPS ;



void sii9293_init_reigster(void);
void sii9293_HDCP_init(void);

void *sii9293_handler(void *p);
UINT8 sii9293_HAL_ReadByte(UINT16 u16_index);

#endif
