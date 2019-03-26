#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define IR_INFO_BUF_SIZE 64

#define IR_DATA_BUF_SIZE 256

#define CLK_WAVE_MIN (23*100)
#define CLK_WAVE_MAX (29*100)
#define CLK_WAVE_MID (26*100) //26us

#define CLK_DATA_MIN (300*100)

#define MIN_DATA_CNT 66
#define MAX_DATA_CNT 198

#define TIMER_LOAD_COUNT (400*1000*100) //400ms
#define TIMER_WAIT_COUNT (1000*1000*100) //1000ms

#define FREQ_OFFSET (IR_DATA_BUF_SIZE - 32)

enum
{
    STATE_IDLE = 0,
    STATE_RECV_START,
    STATE_RECV_DONE,
    STATE_SEND_START,
    STATE_SEND_DONE,
};

struct ir_info_s
{
    int buf;
    int buf_ck;
    int state;
    int data_cnt; 
    int wave_cnt; 
    int data_max; 
};

extern int ir_init(void);
extern SL_POINTER get_ir(SL_POINTER Args);
extern SL_POINTER send_ir(SL_POINTER Args);

