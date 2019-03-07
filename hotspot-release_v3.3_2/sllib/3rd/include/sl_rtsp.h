#ifndef _SL_RTSP_H_
#define _SL_RTSP_H_
#ifdef __cplusplus
extern "C" {
#endif

//#define HANG_PAI

#ifdef HANG_PAI
#define MULTICAST
#define AUDIO_MULTICAST
#define REQUEST_STREAMING_OVER_TCP False
#define AUDIO_REQUEST_STREAMING_OVER_TCP False
#else
#if 0
#define AUDIO_REQUEST_STREAMING_OVER_TCP False
#define REQUEST_STREAMING_OVER_TCP False
#else
#define AUDIO_REQUEST_STREAMING_OVER_TCP True
#define REQUEST_STREAMING_OVER_TCP True
#endif
#endif

#define RINGBUFF
typedef enum _RTSP_STATE_e
{
	SERVER_NOT_CONNECT,
	SERVER_CONNECT,
	SERVER_NOT_RUNNING,
	SERVER_RUNNING
}RTSP_STATE_e;

typedef enum _NET_INTERFACE
{
	INTERFACE_ETH0,
	INTERFACE_WLAN0,
}NET_INTERFACE;

typedef struct _server_param
{
	NET_INTERFACE interface;
	unsigned int multicast;
	char ipMulticastAddr[32];
	char ipMulticastAddrAudio[32];
	
}server_param_t;

typedef struct _client_param
{
	NET_INTERFACE interface;
	unsigned int overTCP;
}client_param_t;

typedef int (*pfmdevcb)(unsigned char *pframe, int  size);
typedef int (*pull)(unsigned char *buffer, int len, int channel);
typedef int (*audio_pull)(unsigned char *buffer, int len);
typedef int (*push)(unsigned char *buffer);

void *StartRtsp(void *p);
int register_pull_method(pull p1,audio_pull p3, push p2);
void rtsp_audio_config(unsigned int fs, unsigned int bits, unsigned int chns); 
int PushToRingBuffer(unsigned char *buffer, int dlen);
int PullFromRingBuffer(unsigned char *buffer, int dlen);
int CountRingBufferUsed();

int rtsp_open(char const* progName, char const* rtspURL, pfmdevcb mdev_cb, client_param_t * param);
int rtsp_open_audio(char const* progName, char const* rtspURL, pfmdevcb mdev_cb, client_param_t * param);
void rtsp_audio_client_config(unsigned int fs, unsigned int audio_bits, unsigned int chns);  

RTSP_STATE_e rtsp_getState();
void rtsp_setState(RTSP_STATE_e state1);
void rtsp_close();
void rtsp_audio_close();
int PCMServerStart(int argc, char** argv);
void PcmServerPlay(void* penv, int argc, char** argv, audio_pull audio_pull_method);
int PCMPush(unsigned char * buff, unsigned int size);
int AACPCMPush(unsigned char * buff, unsigned int size);
int pullFromByteStreamMemoryBuffer(unsigned char * buff, unsigned int size);
int getByteStreamMemoryBufferCnt();
#ifdef __cplusplus
}
#endif

#endif
