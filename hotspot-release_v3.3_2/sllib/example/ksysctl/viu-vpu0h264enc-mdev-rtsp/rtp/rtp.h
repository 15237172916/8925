#ifndef _RTP_H_
#define _RTP_H_

#define PACKET_BUFFER_END            (unsigned int)0x00000000  
  
#define MAX_RTP_PKT_LENGTH     1400  
  
#define DEST_IP                "192.168.1.193"  
#define DEST_PORT            1234  
  
#define H264                    96
  
#define LENGTH_OF_LISTEN_QUEUE     20
  
typedef struct   
{  
    /**//* byte 0 */  
    unsigned char csrc_len:4;        /**//* expect 0 */  
    unsigned char extension:1;        /**//* expect 1, see RTP_OP below */  
    unsigned char padding:1;        /**//* expect 0 */  
    unsigned char version:2;        /**//* expect 2 */  
    /**//* byte 1 */  
    unsigned char payload:7;        /**//* RTP_PAYLOAD_RTSP */  
    unsigned char marker:1;        /**//* expect 1 */  
    /**//* bytes 2, 3 */  
    unsigned short seq_no;              
    /**//* bytes 4-7 */  
    unsigned  long timestamp;          
    /**//* bytes 8-11 */  
    unsigned long ssrc;            /**//* stream number is used here. */  
} RTP_FIXED_HEADER;  
  





#endif



