#ifndef _SL_OS_H_
#define _SL_OS_H_

#include <sl_types.h>
#include <sl_error.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void*		   SLOS_HANDLE;

SL_S32 SLOS_Allocate(SL_U32 Bytes,SL_POINTER * Memory);
SL_S32 SLOS_Free(SL_POINTER Memory);

typedef SL_U32 SLTHREAD_Handle_t;
typedef SL_POINTER (*SLTHREAD_Func_t) (SL_POINTER Args);
SL_S32 SLOS_CreateThread(SLTHREAD_Handle_t* Handle, SLTHREAD_Func_t ThreadFunc, SL_POINTER Args, SL_BOOL Detach);
SL_S32 SLOS_ExitThread(SLTHREAD_Handle_t Handle);
SL_S32 SLOS_JoinThread(SLTHREAD_Handle_t Handle, SL_POINTER Ret);

SL_S32 SLOS_CreateMutex(SL_POINTER * Mutex);
SL_S32 SLOS_DeleteMutex(SL_POINTER Mutex);
SL_S32 SLOS_AcquireMutex(SL_POINTER Mutex,SL_U32 Timeout);
SL_S32 SLOS_ReleaseMutex(SL_POINTER Mutex);

SL_S32 SLOS_CreateCond(SL_POINTER * Cond);
SL_S32 SLOS_DeleteCond(SL_POINTER Cond);
SL_S32 SLOS_WaitCond(SL_POINTER Cond, SL_POINTER Mutex);
SL_S32 SLOS_TimedWaitCond(SL_POINTER Cond, SL_POINTER Mutex, SL_S32 Miliseconds);
SL_S32 SLOS_CondSignal(SL_POINTER Cond);
SL_S32 SLOS_CondBroadcast(SL_POINTER Cond);

SL_S32 SLOS_Delay(SL_U32 Delay);
SL_U32 SLOS_GetTicks(void);
SL_S32 SLOS_GetTime(SL_U32* Time);

SL_S32 SLOS_MemCopy(SL_POINTER Destination,SL_POINTER Source,SL_U32 Bytes);
SL_S32 SLOS_ZeroMemory(SL_POINTER Memory,SL_U32 Bytes);
SL_S32 SLOS_MemFill(SL_POINTER Memory,SL_U32 Filler,SL_U32 Bytes);
SL_S32 SLOS_StrLen(const SL_STRING String,SL_U32 * Length);
SL_S32 SLOS_StrFindReverse(SL_STRING String,SL_U8 Character,SL_STRING * Output);
SL_S32 SLOS_StrCopySafe(SL_STRING Destination,SL_U32 DestinationSize,SL_STRING Source);
SL_S32 SLOS_StrCatSafe(SL_STRING Destination,SL_U32 DestinationSize,SL_STRING Source);
SL_S32 SLOS_StrCmp(SL_STRING String1,SL_STRING String2);
SL_S32 SLOS_StrNCmp(SL_STRING String1,SL_STRING String2,SL_U32 Count);
SL_S32 SLOS_StrToFloat(SL_STRING String,SL_FLOAT * Float);
SL_S32 SLOS_StrToInt(SL_STRING String,SL_S32 * Int);
SL_S32 SLOS_MemCmp(SL_POINTER Memory1,SL_POINTER Memory2,SL_U32 Bytes);

#ifdef SLOS_LOAD_DLIB_SUPPORT
SL_S32 SLOS_LoadLibrary(SL_STRING Library,SLOS_HANDLE *Handle);
SL_S32 SLOS_FreeLibrary(SLOS_HANDLE Handle);
SL_S32 SLOS_GetProcAddress(SLOS_HANDLE Handle,SL_STRING Name,SL_POINTER * Function);
#endif /* SLOS_LOAD_DLIB_SUPPORT */

SL_String_t SLOS_GetEnv(const SL_String_t Name);

#ifdef __cplusplus
}
#endif

#endif

