#ifndef __IR_H_
#define __IR_H
#include <sl_types.h>

#define IR_SERVER_PORT    7998
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024

#if 1
#define IR_DATA_LENGTH 2040
#define IR_DATA_NUM 2
#else
#define IR_DATA_LENGTH 1360
#define IR_DATA_NUM 3
#endif
int init_dsp_ir(void);
SL_POINTER  get_ir(SL_POINTER Args);


#endif