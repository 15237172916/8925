
#include <handle.h>
#include "sl_os.h"
#include "sl_msg.h"

#define MAX_MESSAGES (256)

typedef struct _msg_queue{
	SL_U32 thread;
	SL_POINTER mutex;
	SL_POINTER cond;
	struct list_head free_queue;
	struct list_head queue;
	SLMSG_Message_t message_pool[MAX_MESSAGES];
}MSG_QUEUE;

static SL_Verify_t verifyData = {
	size : sizeof(MSG_QUEUE),
	name : "silan message module",
	version : "0.0.1"
};

static SL_HandlePool_t* msgHandlePool = SL_NULL;

static SL_ErrorCode_t destroyMsgQueue(SL_Handle_t handle){
	SL_ErrorCode_t err = SL_NO_ERROR;
	MSG_QUEUE* queue = SL_NULL;

	err = SL_GetPrivate(handle, &verifyData, (SL_POINTER)queue);
	if (SL_NO_ERROR != err){
		return SL_NO_ERROR;
	}

	if (0 != queue->mutex){
		SLOS_DeleteMutex(queue->mutex);
	}

	if (0 != queue->cond){
		SLOS_DeleteCond(queue->cond);
	}

	return SL_NO_ERROR;
}

SL_ErrorCode_t SLMSG_Init (void){
	return SL_CreateHandlePool(&verifyData, 128, destroyMsgQueue, &msgHandlePool);
}

SL_ErrorCode_t SLMSG_Term (void){
	return SL_DestroyHandlePool(msgHandlePool);
}

SL_ErrorCode_t SLMSG_Open (SL_Handle_t* Handle){
	SL_ErrorCode_t ret = SL_NO_ERROR;
	SL_Handle_t hdl;
	MSG_QUEUE* queue = SL_NULL;
	int i = 0;

	*Handle = SL_NULL;

	ret = SL_CreateHandleFromPool(msgHandlePool, SL_FALSE, &hdl);
	if (SL_NO_ERROR != ret){
		return ret;
	}

	ret = SL_GetPrivate(hdl, &verifyData, (SL_POINTER)&queue);
	if (SL_NO_ERROR != ret){
		SL_DestroyHandleByPool(msgHandlePool, hdl);
		return ret;
	}

	SLOS_ZeroMemory((SL_POINTER)queue, sizeof(MSG_QUEUE));

	ret = SLOS_CreateMutex(&queue->mutex);
	if (SL_NO_ERROR != ret){
		SL_DestroyHandleByPool(msgHandlePool, hdl);
		return ret;
	}

	ret = SLOS_CreateCond(&queue->cond);
	if (SL_NO_ERROR != ret){
		SLOS_DeleteMutex(queue->mutex);
		SL_DestroyHandleByPool(msgHandlePool, hdl);
		return ret;
	}

	INIT_LIST_HEAD(&queue->free_queue);
	INIT_LIST_HEAD(&queue->queue);

	for (i = 0; i < MAX_MESSAGES; i ++){
		INIT_LIST_HEAD(&(queue->message_pool[i].list));
		list_add_tail(&(queue->message_pool[i].list), &queue->free_queue);
	}

	SL_ReleasePrivate(hdl, (SL_POINTER*)queue);
	*Handle = hdl;
	return ret;
}

SL_ErrorCode_t SLMSG_Close(SL_Handle_t Handle){
	return SL_DestroyHandleByPool(msgHandlePool, Handle);
}

SL_ErrorCode_t SLMSG_PollMessage(SL_Handle_t Handle, SLMSG_Message_t* Message){
	SL_ErrorCode_t err = SL_NO_ERROR;
	SL_ErrorCode_t ret = SL_NO_ERROR;
	MSG_QUEUE* queue = SL_NULL;
	SLMSG_Message_t* msg;

	err = SL_GetPrivate(Handle, &verifyData, (SL_POINTER)&queue);
	if (SL_NO_ERROR != err){
		return ret;
	}

	ret = SLOS_AcquireMutex(queue->mutex, SL_INFINITE);

	if (list_empty(&queue->queue)){
		SLOS_ReleaseMutex(queue->mutex);
		return SL_ERROR_NO_MORE;
	}
	msg = (SLMSG_Message_t*)list_entry(queue->queue.next, SLMSG_Message_t, list);

	list_del_init(&msg->list);
	*Message = *msg;
	list_add_tail(&msg->list, &queue->free_queue);
	ret = SLOS_ReleaseMutex(queue->mutex);
	err = SL_ReleasePrivate(Handle, (SL_POINTER*)queue);
	return ret;
}

SL_ErrorCode_t SLMSG_WaitMessage(SL_Handle_t Handle, SLMSG_Message_t* Message, SL_S32 Miliseconds){
	SL_ErrorCode_t err = SL_NO_ERROR;
	SL_ErrorCode_t ret = SL_NO_ERROR;
	MSG_QUEUE* queue = SL_NULL;
	SLMSG_Message_t* msg;

	err = SL_GetPrivate(Handle, &verifyData, (SL_POINTER)&queue);
	if (SL_NO_ERROR != err){
		return ret;
	}

	ret = SLOS_AcquireMutex(queue->mutex, SL_INFINITE);

	if (list_empty(&queue->queue)){
		ret = SLOS_TimedWaitCond(queue->cond, queue->mutex, Miliseconds);
	}

	if (SL_NO_ERROR == ret){
		if (list_empty(&queue->queue)){
			ret = SL_ERROR_NO_MORE;
		}else{
			msg = (SLMSG_Message_t*)list_entry(queue->queue.next, SLMSG_Message_t, list);

			list_del_init(&msg->list);
			*Message = *msg;
			list_add_tail(&msg->list, &queue->free_queue);
		}
	}

	SLOS_ReleaseMutex(queue->mutex);
	SL_ReleasePrivate(Handle, (SL_POINTER*)queue);
	return ret;
}

SL_ErrorCode_t SLMSG_PostMessage(SL_Handle_t Handle, SLMSG_Message_t* Message){
	SL_ErrorCode_t err = SL_NO_ERROR;
	SL_ErrorCode_t ret = SL_NO_ERROR;
	MSG_QUEUE* queue = SL_NULL;
	SLMSG_Message_t* msg;

	err = SL_GetPrivate(Handle, &verifyData, (SL_POINTER)&queue);
	if (SL_NO_ERROR != err){
		return ret;
	}

	ret = SLOS_AcquireMutex(queue->mutex, SL_INFINITE);

	if (list_empty(&queue->free_queue)){
		SLOS_ReleaseMutex(queue->mutex);
		return SL_ERROR_NO_MEMORY;
	}

	msg = (SLMSG_Message_t*)list_entry(queue->free_queue.next, SLMSG_Message_t, list);

	list_del_init(&msg->list);
	*msg = *Message;
	INIT_LIST_HEAD(&msg->list);
	list_add_tail(&msg->list, &queue->queue);

	SLOS_CondSignal(queue->cond);
	ret = SLOS_ReleaseMutex(queue->mutex);
	err = SL_ReleasePrivate(Handle, (SL_POINTER*)queue);
	return ret;
}

