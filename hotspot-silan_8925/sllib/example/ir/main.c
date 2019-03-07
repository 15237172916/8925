#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>

#include <sl_ir.h>

#define IR_DEVICE_NAME "/dev/event"
#define IR_DEV "sl-ir"

#define EVIOCGNAME(len)         _IOC(_IOC_READ, 'E', 0x06, len)     

typedef struct
{
    struct timeval time;
    SL_U16 type;
    SL_U16 code;    
    SL_S32 value;
}ir_event_t;

typedef struct 
{
    SL_S32 fd;        /*device fd*/
}ir_handle_t;

static ir_handle_t m_ir_handle = {0};
static ir_handle_t *ir_handle = &m_ir_handle;

SL_ErrorCode_t SLIR_Open(const SLIR_OpenParams_t *OpenParams)
{
    SL_U8 i;
    char *device = NULL;
    char name[20];

    if ((OpenParams == NULL) || ((OpenParams->device) != 0)) 
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    for (i = 0; i < 4; i++)
    {
        sprintf(device, "%s%d", IR_DEVICE_NAME, i);    
        
        ir_handle->fd = open (device, O_RDWR); 
        if (ir_handle->fd < 0)
        {
            continue;
        }
        
        if( (ioctl(ir_handle->fd, EVIOCGNAME(sizeof(name)), name)) < 0)
        {
            continue;
        }

        if (strncmp(name, IR_DEV, strlen(IR_DEV)) == 0)
        {
            break;
        }
        else
        {
            close(ir_handle->fd);
        }
    }

    return SL_NO_ERROR; 
}

SL_ErrorCode_t SLIR_Close()
{
	SL_ErrorCode_t errCode = SL_NO_ERROR;
	if ((close(ir_handle->fd)) < 0)
	{
		errCode = SL_ERROR_INVALID_HANDLE; 
	}
	return SL_ERROR_INVALID_HANDLE; 
}

SL_ErrorCode_t SLIR_Read(SL_U16 *irCode)
{
    ir_event_t ir;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if (irCode == NULL)
	{
		return SL_ERROR_BAD_PARAMETER;
	}

	if ((read(ir_handle->fd, &ir, sizeof(ir_event_t))) <= 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*irCode = ir.code;

    return errCode;
} 


int main(void)
{
    SLIR_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;

    SL_U16 irCode;
    
	nOpenParam = (SLIR_OpenParams_t *)malloc(sizeof(SLIR_OpenParams_t));
    nOpenParam->device = SLIR_DEVICE1;

    errCode = SLIR_Open(nOpenParam);
    if(errCode != 0)
    {
        printf("SLIR_Open error\n");
        return -1;
    }
    
    while(1)
    {
        errCode = SLIR_Read(&irCode);
        if(errCode != 0)
        {
            printf("SLIR_Read error\n");
            return -1;
        }
        printf("irCode:    %d\n", irCode);
    }

    errCode = SLIR_Close();
    if(errCode != 0)
    {
        printf("SLIR_Close error\n");
        return -1;
    }

    return 0;
}

