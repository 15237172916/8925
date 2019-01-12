
#include <sl_types.h>
void *app_tx_signal_ch_main(void *arg);
typedef struct
{
    SL_BOOL bCmdFlag;
    unsigned char iCmdData[9];
}COMMAND;

typedef struct
{
	SL_BOOL bSendFlag; //SL_FALSE not show, SL_TRUE show
	unsigned char uCmdBuf[9];
}REMOTECMD;

REMOTECMD g_RemoteCmd;

COMMAND g_TxCommand;




