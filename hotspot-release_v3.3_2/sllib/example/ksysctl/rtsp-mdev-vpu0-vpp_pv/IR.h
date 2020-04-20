#ifndef __IR_H_
#define __IR_H_
#include <sl_types.h>
#if 1
#define IR_DATA_LENGTH 2040
#define IR_DATA_NUM 2
#else
#define IR_DATA_LENGTH 1360
#define IR_DATA_NUM 3
#endif

#define IR_CLIENT_PORT  7998
#define BUFFER_SIZE 1024

typedef struct
{
    unsigned char num[IR_DATA_LENGTH];
}Node;
//static SL_U32 dsp_dev;


SL_POINTER  send_ir(SL_POINTER Args);

#endif