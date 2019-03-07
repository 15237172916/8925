#ifndef __HANDLE_H__
#define __HANDLE_H__

#include "sl_types.h"
#include "sl_error.h"
#include "list.h"

#define MAX_MODULE_NAME_SIZE 32
#define MAX_MODULE_VERSION_SIZE 32

/*
 * Handle functions
 *
 */

typedef struct SL_Verify_t{
	SL_S32 size;
	SL_STRING name;
	SL_STRING version;
}SL_Verify_t;

SL_ErrorCode_t SL_CreateHandle(SL_Verify_t* verifyData,	SL_BOOL hasMutex, SL_Handle_t* handle);
SL_ErrorCode_t SL_DestroyHandle(SL_Handle_t handle);
SL_ErrorCode_t SL_GetPrivate(SL_Handle_t handle, SL_Verify_t* verifyData, SL_POINTER* data);
SL_ErrorCode_t SL_ReleasePrivate(SL_Handle_t handle, SL_POINTER* data);
SL_ErrorCode_t SL_VerifyHandle(SL_Handle_t handle, SL_Verify_t* verifyData);


/*
 * HandlePool functions
 *
 */
typedef SL_ErrorCode_t (*SL_DestroyCallback_t) (SL_Handle_t handle);

typedef struct SL_HandlePool_t{
	SL_Verify_t* verifyData;
	SL_DestroyCallback_t destroy;
	SL_S32		max;
	SL_Handle_t* handles;
}SL_HandlePool_t;

SL_ErrorCode_t SL_CreateHandlePool(SL_Verify_t* verifyData, 
							SL_S32 maxHandles,
							SL_DestroyCallback_t func,
							SL_HandlePool_t** handlePool);
SL_ErrorCode_t SL_DestroyHandlePool(SL_HandlePool_t* handlePool);
SL_ErrorCode_t SL_CreateHandleFromPool(SL_HandlePool_t* handlePool, SL_BOOL hasMutex, SL_Handle_t* handle);
SL_ErrorCode_t SL_DestroyHandleByPool(SL_HandlePool_t* handlePool, SL_Handle_t handle);

#endif
