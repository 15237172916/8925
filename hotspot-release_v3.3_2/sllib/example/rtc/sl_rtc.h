
/**
 * @file sl_rtc.h
 * @brief provide the ports for rtc devices
 * @author Chen Jianneng
 * @version SL_RTC-REL_1.1.0
 * @date 2012-10-08
 */

#ifndef _SL_RTC_H_
#define _SL_RTC_H_    

#include <sl_types.h>
#include <sl_error.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SLRTCDevice 
{
    SLRTC1,  /*rtc0*/
};

typedef struct _SLRTC_OpenParms_t
{
    enum SLRTCDevice device;
}SLRTC_OpenParams_t;

typedef struct rtc_time 
{
    SL_S32 tm_sec;
    SL_S32 tm_min;
    SL_S32 tm_hour;
    SL_S32 tm_mday;
    SL_S32 tm_mon;
    SL_S32 tm_year;
    SL_S32 tm_wday;
    SL_S32 tm_yday;
    SL_S32 tm_isdst;
}SL_rtc_t;

SL_ErrorCode_t SLRTC_Open(const SLRTC_OpenParams_t *OpenParams);

SL_ErrorCode_t SLRTC_Close();

SL_ErrorCode_t SLRTC_Read(SL_rtc_t *Read_rtc);

SL_ErrorCode_t SLRTC_Write(SL_rtc_t Write_rtc);

SL_ErrorCode_t SLRTC_ReadYear(SL_S32 *Year);

SL_ErrorCode_t SLRTC_SetYear(SL_S32 Year);
    
SL_ErrorCode_t SLRTC_ReadMon(SL_S32 *Month);

SL_ErrorCode_t SLRTC_SetMon(SL_S32 Month);

SL_ErrorCode_t SLRTC_ReadDay(SL_S32 *Day);

SL_ErrorCode_t SLRTC_SetDay(SL_S32 Day);

SL_ErrorCode_t SLRTC_ReadWeekday(SL_S32 *Weekday);

SL_ErrorCode_t SLRTC_SetWeekday(SL_S32 Weekday);

SL_ErrorCode_t SLRTC_ReadHour(SL_S32 *Hour);

SL_ErrorCode_t SLRTC_SetHour(SL_S32 Hour);

SL_ErrorCode_t SLRTC_ReadMin(SL_S32 *Minute);

SL_ErrorCode_t SLRTC_SetMin(SL_S32 Minute);

SL_ErrorCode_t SLRTC_ReadSec(SL_S32 *Second);

SL_ErrorCode_t SLRTC_SetSec(SL_S32 Second);

#ifdef __cplusplus
}
#endif

#endif

