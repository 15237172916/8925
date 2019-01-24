#include <stdio.h>
#include "sl_types.h"
#include "sl_error.h"
#include "sl_os.h"
#include "handle.h"

typedef struct _handle{
	SL_S32 id;
	SL_Verify_t* verifyData;
	SL_POINTER private;
	SL_BOOL hasMutex;
	SL_POINTER mutex;
}Handle_t;

SL_ErrorCode_t SL_CreateHandle(SL_Verify_t* verifyData,	SL_BOOL hasMutex, SL_Handle_t* handle){
	SL_ErrorCode_t ret;
	Handle_t* hdl;
	if (SL_NULL == handle 
			|| SL_NULL == verifyData 
			|| verifyData->size == 0){
		return SL_ERROR_BAD_PARAMETER;
	}

	ret = SLOS_Allocate(sizeof(Handle_t) + verifyData->size, (SL_POINTER)&hdl);
	if (SL_NO_ERROR != ret){
		return SL_ERROR_NO_MEMORY;
	}

	hdl->id = -1;
	hdl->verifyData = verifyData;
	hdl->private = (SL_POINTER)(hdl + 1);

	if (SL_TRUE == hasMutex){
		SLOS_CreateMutex(&hdl->mutex);
	}

	*handle = (SL_Handle_t)(~(SL_U32)hdl);
	return SL_NO_ERROR;
}


SL_ErrorCode_t SL_DestroyHandle(SL_Handle_t handle){
	Handle_t* hdl = (Handle_t*) ~handle;

	if (SL_NULL == hdl){
		return SL_ERROR_BAD_PARAMETER;
	}

	if (hdl->id >= 0){
		return SL_ERROR_INVALID_HANDLE;
	}

	if (SL_TRUE == hdl->hasMutex){
		SLOS_DeleteMutex(hdl->mutex);
		hdl->hasMutex = SL_FALSE;
	}

	hdl->verifyData = SL_NULL;
	hdl->private = SL_NULL;

	SLOS_Free((SL_POINTER) hdl);

	return SL_NO_ERROR;
}

static SL_ErrorCode_t setId(SL_Handle_t handle, SL_U32 id){
	Handle_t* hdl = (Handle_t*) ~handle;

	if (SL_NULL == hdl){
		return SL_ERROR_BAD_PARAMETER;
	}

	hdl->id = id;

	return SL_NO_ERROR;
}


SL_ErrorCode_t SL_GetPrivate(SL_Handle_t handle, SL_Verify_t* verifyData, SL_POINTER* data){
	Handle_t* hdl = (Handle_t*) ~handle;

	if (SL_NULL == hdl || SL_NULL == verifyData || SL_NULL == data){
		return SL_ERROR_BAD_PARAMETER;
	}

	if (verifyData != hdl->verifyData){
		return SL_ERROR_INVALID_HANDLE;
	}

	if (SL_TRUE == hdl->hasMutex){
		SLOS_AcquireMutex(hdl->mutex, -1);
	}

	*data = hdl->private;

	return SL_NO_ERROR;
}

SL_ErrorCode_t SL_ReleasePrivate(SL_Handle_t handle, SL_POINTER* data){
	Handle_t* hdl = (Handle_t*) ~handle;

	if (SL_NULL == hdl || SL_NULL == data){
		return SL_ERROR_BAD_PARAMETER;
	}

	if (SL_TRUE == hdl->hasMutex){
		SLOS_ReleaseMutex(hdl->mutex);
	}

	return SL_NO_ERROR;
}

SL_ErrorCode_t SL_VerifyHandle(SL_Handle_t handle, SL_Verify_t* verifyData){
	Handle_t* hdl = (Handle_t*) ~handle;

	if (SL_NULL == hdl || SL_NULL == verifyData){
		return SL_ERROR_BAD_PARAMETER;
	}

	if (hdl->verifyData != verifyData){
		return SL_ERROR_INVALID_HANDLE;
	}

	return SL_NO_ERROR;
}

SL_ErrorCode_t SLHANDLE_GetModuleName(SL_Handle_t handle, SL_STRING* name){
	Handle_t* hdl = (Handle_t*) ~handle;

	if (SL_NULL == hdl || SL_NULL == name){
		return SL_ERROR_BAD_PARAMETER;
	}

	if (SL_NULL == hdl->verifyData){
		return SL_ERROR_INVALID_HANDLE;
	}

	*name = hdl->verifyData->name;
	return SL_NO_ERROR;
}

SL_ErrorCode_t SLHANDLE_GetModuleVersion(SL_Handle_t handle, SL_STRING* version){
	Handle_t* hdl = (Handle_t*) ~handle;

	if (SL_NULL == hdl || SL_NULL == version){
		return SL_ERROR_BAD_PARAMETER;
	}

	if (SL_NULL == hdl->verifyData){
		return SL_ERROR_INVALID_HANDLE;
	}

	*version = hdl->verifyData->version;
	return SL_NO_ERROR;
}


SL_ErrorCode_t SL_CreateHandlePool(SL_Verify_t* verifyData, 
							SL_S32 maxHandles,
							SL_DestroyCallback_t func,
							SL_HandlePool_t** handlePool){
	SL_HandlePool_t* hdlPool;
	SL_ErrorCode_t ret;
	if (SL_NULL == verifyData || 0 >= maxHandles || SL_NULL == handlePool){
		return SL_ERROR_BAD_PARAMETER;
	}

	ret = SLOS_Allocate(sizeof(SL_HandlePool_t) + sizeof(Handle_t*) * maxHandles, (SL_POINTER)&hdlPool);
	if (SL_NO_ERROR != ret){
		return SL_ERROR_NO_MEMORY;
	}

	SLOS_ZeroMemory((SL_POINTER)hdlPool, sizeof(SL_HandlePool_t) + sizeof(Handle_t*) * maxHandles);
	hdlPool->verifyData = verifyData;
	hdlPool->destroy = func;
	hdlPool->max = maxHandles;
	hdlPool->handles = (SL_Handle_t*)(hdlPool + 1);

	*handlePool = hdlPool;

	return SL_NO_ERROR;
}

SL_ErrorCode_t SL_DestroyHandlePool(SL_HandlePool_t* handlePool){
	SL_ErrorCode_t err = SL_NO_ERROR;
	SL_S32 i = 0;
	if (SL_NULL == handlePool){
		return SL_ERROR_BAD_PARAMETER;
	}

	for (i = 0; i < handlePool->max; i ++){
		if (SL_NULL != handlePool->handles[i]){
			err = SL_DestroyHandleByPool(handlePool, handlePool->handles[i]);
		}
	}

	if (SL_NO_ERROR == err){
		SLOS_Free(handlePool);
	}

	return err;
}

SL_ErrorCode_t SL_CreateHandleFromPool(SL_HandlePool_t* handlePool, SL_BOOL hasMutex, SL_Handle_t* handle){
	SL_ErrorCode_t err;
	SL_Handle_t hdl;
	SL_S32 i = 0;

	if (SL_NULL == handlePool || SL_NULL == handle){
		return SL_ERROR_BAD_PARAMETER;
	}

	for (i = 0; i < handlePool->max; i ++){
		if (SL_NULL == handlePool->handles[i]){
			err = SL_CreateHandle(handlePool->verifyData, hasMutex, (SL_Handle_t*)&hdl);
			if (SL_NO_ERROR != err){
				return err;
			}
			setId(hdl, i);
			handlePool->handles[i] = hdl;
			*handle = hdl;
			return SL_NO_ERROR;
		}
	}

	return SL_ERROR_NO_FREE_HANDLE;
}

SL_ErrorCode_t SL_DestroyHandleByPool(SL_HandlePool_t* handlePool, SL_Handle_t handle){
	SL_ErrorCode_t err = SL_NO_ERROR;
	Handle_t* hdl = (Handle_t*) ~handle;
	SL_S32 id;

	if (SL_NULL == handlePool || SL_NULL == hdl){
		return SL_ERROR_BAD_PARAMETER;
	}

	id = hdl->id;

	if (id < 0 || handlePool->handles[id] != handle){
		return SL_ERROR_INVALID_HANDLE;
	}
	
	if (SL_NULL != handlePool->destroy){
		err = handlePool->destroy(handle);
	}

	if (SL_NO_ERROR == err){
		err = SL_DestroyHandle(handle);
	}

	if (SL_NO_ERROR == err){ 
		handlePool->handles[id] = SL_NULL;
	}
	return err;
}

