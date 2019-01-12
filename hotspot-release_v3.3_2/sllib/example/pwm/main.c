#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <sl_pwm.h>

#define PWM_DEVICE_NAME "/sys/class/backlight/pwm-backlight/brightness"

static SL_S32 fd;        /*device fd*/

SL_ErrorCode_t SLPWM_Setvalue(SL_S32 Value)
{
    char buffer[20];
    SL_ErrorCode_t errCode = SL_NO_ERROR;
    SL_S32 bytes;

    if (Value < 0 || Value > 272)
    {
        return SL_ERROR_BAD_PARAMETER;
    }
    bytes = sprintf(buffer, "%d\n", Value);
    if ((write(fd, buffer, bytes)) < 0)
    {
		errCode = SL_ERROR_GENERIC_IO;
    } 
    return errCode;
}

int main(void)
{
    SL_ErrorCode_t errCode;
    SL_U8 i;
	fd = open (PWM_DEVICE_NAME, O_RDWR); 
	if (fd < 0)
	{
		errCode = SL_ERROR_UNKNOWN_DEVICE;
	}
    for(i = 0; i < 4; i++)
    {
        errCode = SLPWM_Setvalue(i*50);
        if(errCode != 0)
        {
            printf("SLPWM_SetValue error\n");
            return -1;
        }
        sleep(1);
    }
	if ((close(fd)) < 0)
    {
		return SL_ERROR_INVALID_HANDLE; 
    }
    return SL_NO_ERROR;
}

