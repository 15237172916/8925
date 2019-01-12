#include <stdio.h>
#include <string.h>
#include <sl_rtc.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>

#define RTC_DEVICE_NAME "/dev/rtc"

#define RTC_RD_TIME   _IOR('p', 0x09, struct rtc_time) /*Read RTC time*/
#define RTC_SET_TIME  _IOW('p', 0x0a, struct rtc_time) /*Set RTC time*/

SL_S32 fd;        /*device fd*/

char *week[7] = {"Mon.", "Tues.", "Wed.", "Thurs.", "Fri.", "Sat.", "Sun."};

SL_ErrorCode_t SLRTC_Open(const SLRTC_OpenParams_t *OpenParams)
{
    char *device = NULL;
    
    if ((OpenParams == NULL) || ((OpenParams->device) < 0) || ((OpenParams->device) > 3))
    {
        return SL_ERROR_BAD_PARAMETER;
    }

    sprintf(device, "%s%d", RTC_DEVICE_NAME, OpenParams->device);    

    fd = open(device, O_RDWR);
    
    if (fd < 0)
    {
        return SL_ERROR_UNKNOWN_DEVICE;
    }           
    return SL_NO_ERROR; 
}

SL_ErrorCode_t SLRTC_Close()
{
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if (fd < 0)
	{
		errCode = SL_ERROR_INVALID_HANDLE; 
	}
	return errCode;
}

SL_ErrorCode_t SLRTC_Read(SL_rtc_t *Read_rtc)
{
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if(Read_rtc == NULL)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, Read_rtc)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

    return errCode;
} 

SL_ErrorCode_t SLRTC_Write(SL_rtc_t Write_rtc)
{
	SL_ErrorCode_t errCode = SL_NO_ERROR;
	if ((ioctl(fd, RTC_SET_TIME, &Write_rtc)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	return errCode;
}

SL_ErrorCode_t SLRTC_ReadYear(SL_S32 *Year)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*Year = rtc_tm.tm_year;

    return errCode;
}

SL_ErrorCode_t SLRTC_SetYear(SL_S32 Year)
{
    SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if(Year < 0)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	rtc_tm.tm_year = Year;

	if ((ioctl(fd, RTC_SET_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	return errCode;
}

SL_ErrorCode_t SLRTC_ReadMon(SL_S32 *Month)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*Month = rtc_tm.tm_mon;

	return errCode;
}

SL_ErrorCode_t SLRTC_SetMon(SL_S32 Month)
{
    SL_rtc_t rtc_tm;
    SL_ErrorCode_t errCode = SL_NO_ERROR;
    
	if(Month < 0 || Month > 13)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	rtc_tm.tm_mon = Month;

	if ((ioctl(fd, RTC_SET_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	return errCode;
}

SL_ErrorCode_t SLRTC_ReadDay(SL_S32 *Day)
{
    SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*Day = rtc_tm.tm_mday;

	return errCode;
}

SL_ErrorCode_t SLRTC_SetDay(SL_S32 Day)
{    
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if(Day < 0 || Day > 31)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	rtc_tm.tm_mday = Day;

	if ((ioctl(fd, RTC_SET_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}
	return errCode;
}

SL_ErrorCode_t SLRTC_ReadWeekday(SL_S32 *Weekday)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*Weekday = rtc_tm.tm_wday;
	return errCode;
}

SL_ErrorCode_t SLRTC_SetWeekday(SL_S32 Weekday)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if(Weekday < 0 || Weekday > 7)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	rtc_tm.tm_wday = Weekday;

	if ((ioctl(fd, RTC_SET_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	return errCode;
}

SL_ErrorCode_t SLRTC_ReadHour(SL_S32 *Hour)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*Hour = rtc_tm.tm_hour;
	return errCode;
}

SL_ErrorCode_t SLRTC_SetHour(SL_S32 Hour)
{
    SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if(Hour < 0 || Hour > 24)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}
	rtc_tm.tm_hour = Hour;

	if ((ioctl(fd, RTC_SET_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}
    return errCode;
}

SL_ErrorCode_t SLRTC_ReadMin(SL_S32 *Minute)
{
    SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*Minute = rtc_tm.tm_min;

	return errCode;
}

SL_ErrorCode_t SLRTC_SetMin(SL_S32 Minute)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if(Minute < 0 || Minute > 60)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	rtc_tm.tm_min = Minute;

	if ((ioctl(fd, RTC_SET_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}
	return errCode;
}

SL_ErrorCode_t SLRTC_ReadSec(SL_S32 *Second)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	*Second = rtc_tm.tm_sec;
	return errCode;
}

SL_ErrorCode_t SLRTC_SetSec(SL_S32 Second)
{
	SL_rtc_t rtc_tm;
	SL_ErrorCode_t errCode = SL_NO_ERROR;

	if(Second < 0 || Second > 60)
	{
		return SL_ERROR_BAD_PARAMETER;
	}
	if ((ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}

	rtc_tm.tm_sec = Second;

	if ((ioctl(fd, RTC_SET_TIME, &rtc_tm)) < 0)
	{
		errCode = SL_ERROR_GENERIC_IO;
	}
	return errCode;
}

SL_ErrorCode_t test_year()
{
    SL_S32 year;
    SL_ErrorCode_t errCode;

    errCode = SLRTC_ReadYear(&year);
    if(errCode != 0)
    {
        printf("SLRTC_ReadYear error\n");
        return -1;
    }
    printf("before set year: %04d\n", year+1900);


    year = 2022-1900;
    
    errCode = SLRTC_SetYear(year);
    if(errCode != 0)
    {
        printf("SLRTC_SetYear error\n");
        return -1;
    }

    errCode = SLRTC_ReadYear(&year);
    if(errCode != 0)
    {
        printf("SLRTC_ReadYear error\n");
        return -1;
    }
    printf("After set year:     %04d\n\n", year+1900);

    return errCode;
}

SL_ErrorCode_t test_month()
{
    SL_S32 month;
    SL_ErrorCode_t errCode;

    errCode = SLRTC_ReadMon(&month);
    if(errCode != 0)
    {
        printf("SLRTC_ReadMon error\n");
        return -1;
    }
    printf("before set month: %02d\n", month);


    month = 2;
    
    errCode = SLRTC_SetMon(month);
    if(errCode != 0)
    {
        printf("SLRTC_SetMon error\n");
        return -1;
    }

    errCode = SLRTC_ReadMon(&month);
    if(errCode != 0)
    {
        printf("SLRTC_ReadMon error\n");
        return -1;
    }
    printf("After set month:  %02d\n\n", month);

    return errCode;
}

SL_ErrorCode_t test_day()
{
    SL_S32 day;
    SL_ErrorCode_t errCode;

    errCode = SLRTC_ReadDay(&day);
    if(errCode != 0)
    {
        printf("SLRTC_ReadDay error\n");
        return -1;
    }
    printf("before set day: %02d\n", day);


    day = 2;
    
    errCode = SLRTC_SetDay(day);
    if(errCode != 0)
    {
        printf("SLRTC_SetDay error\n");
        return -1;
    }

    errCode = SLRTC_ReadDay(&day);
    if(errCode != 0)
    {
        printf("SLRTC_ReadDay error\n");
        return -1;
    }
    printf("After set day:  %02d\n\n", day);

    return errCode;
}

SL_ErrorCode_t test_hour()
{
    SL_S32 hour;
    SL_ErrorCode_t errCode;

    errCode = SLRTC_ReadHour(&hour);
    if(errCode != 0)
    {
        printf("SLRTC_ReadHour error\n");
        return -1;
    }
    printf("before set hour: %02d\n", hour);


    hour = 2;
    
    errCode = SLRTC_SetHour(hour);
    if(errCode != 0)
    {
        printf("SLRTC_SetHour error\n");
        return -1;
    }

    errCode = SLRTC_ReadHour(&hour);
    if(errCode != 0)
    {
        printf("SLRTC_ReadHour error\n");
        return -1;
    }
    printf("After set hour:     %02d\n\n", hour);

    return errCode;
}

SL_ErrorCode_t test_min()
{
    SL_S32 min;
    SL_ErrorCode_t errCode;

    errCode = SLRTC_ReadMin(&min);
    if(errCode != 0)
    {
        printf("SLRTC_ReadMin error\n");
        return -1;
    }
    printf("before set min: %02d\n", min);


    min = 2;
    
    errCode = SLRTC_SetMin(min);
    if(errCode != 0)
    {
        printf("SLRTC_SetMin error\n");
        return -1;
    }

    errCode = SLRTC_ReadMin(&min);
    if(errCode != 0)
    {
        printf("SLRTC_ReadMin error\n");
        return -1;
    }
    printf("After set min:    %02d\n\n", min);

    return errCode;
}

SL_ErrorCode_t test_sec()
{
    SL_S32 sec;
    SL_ErrorCode_t errCode;

    errCode = SLRTC_ReadSec(&sec);
    if(errCode != 0)
    {
        printf("SLRTC_ReadSec error\n");
        return -1;
    }
    printf("before set sec: %02d\n", sec);


    sec = 2;
    
    errCode = SLRTC_SetSec(sec);
    if(errCode != 0)
    {
        printf("SLRTC_SetSec error\n");
        return -1;
    }

    errCode = SLRTC_ReadSec(&sec);
    if(errCode != 0)
    {
        printf("SLRTC_ReadSec error\n");
        return -1;
    }
    printf("After set sec:    %02d\n\n", sec);

    return errCode;
}

SL_ErrorCode_t test_weekday()
{
    SL_S32 weekday;
    SL_ErrorCode_t errCode;

    errCode = SLRTC_ReadWeekday(&weekday);
    if(errCode != 0)
    {
        printf("SLRTC_ReadWeekday error\n");
        return -1;
    }
    printf("before set weekday: %s\n", week[weekday-1]);


    weekday = 5;
    
    errCode = SLRTC_SetWeekday(weekday);
    if(errCode != 0)
    {
        printf("SLRTC_SetWeekday error\n");
        return -1;
    }

    errCode = SLRTC_ReadWeekday(&weekday);
    if(errCode != 0)
    {
        printf("SLRTC_ReadWeekday error\n");
        return -1;
    }
    printf("After set weekday:    %s\n\n", week[weekday-1]);

    return errCode;
}


SL_ErrorCode_t test_rtc()
{
    SL_ErrorCode_t errCode;

    SL_rtc_t rtc_tm;
    
    errCode = SLRTC_Read(&rtc_tm);
    if(errCode != 0)
    {
        printf("SLRTC_Read error\n");
        return -1;
    }
    
    printf("y-m-d: %04d-%02d-%02d  h:m:s: %02d:%02d:%02d  weekday: %s\n",
            1900+rtc_tm.tm_year, rtc_tm.tm_mon, rtc_tm.tm_mday, rtc_tm.tm_hour,
            rtc_tm.tm_min, rtc_tm.tm_sec, week[rtc_tm.tm_wday-1]);

    rtc_tm.tm_year = 2012-1900;
    rtc_tm.tm_mon = 10;
    rtc_tm.tm_mday = 9;
    rtc_tm.tm_hour = 9;
    rtc_tm.tm_min = 9;
    rtc_tm.tm_sec = 9;
    rtc_tm.tm_wday = 2;

    errCode = SLRTC_Write(rtc_tm);
    if(errCode != 0)
    {
        printf("SLRTC_Write error\n");
        return -1;
    }
    
    errCode = SLRTC_Read(&rtc_tm);
    if(errCode != 0)
    {
        printf("SLRTC_Read error\n");
        return -1;
    }
    
    printf("y-m-d: %04d-%02d-%02d  h:m:s: %02d:%02d:%02d  weekday: %s\n\n",
            1900+rtc_tm.tm_year, rtc_tm.tm_mon, rtc_tm.tm_mday, rtc_tm.tm_hour,
            rtc_tm.tm_min, rtc_tm.tm_sec, week[rtc_tm.tm_wday-1]);

    return errCode;
}

int main(void)
{
    SLRTC_OpenParams_t *nOpenParam;
    SL_ErrorCode_t errCode;

    nOpenParam = (SLRTC_OpenParams_t *)malloc(sizeof(SLRTC_OpenParams_t));
    nOpenParam->device = SLRTC1;

    errCode = SLRTC_Open(nOpenParam);
    if(errCode != 0)
    {
        printf("SLRTC_Open error\n");
        return -1;
    }
    
    test_rtc();
     
    test_year();
    
    test_month();
    
    test_day();

    test_hour();

    test_min();
    
    test_sec();
    
    test_weekday();
    
    return 0;
}


