
/**
 * @file sl_log.h
 * @brief silan 线程间Messgae系统，提供内外部线程间消息服务
 * @author Zhu Hao
 * @version SLLOG_0.0.1
 * @date 2012-09-26
 */


#ifndef __SLMSG_H__
#define __SLMSG_H__

#include <sl_types.h>
#include <sl_error.h>
#include "list.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _SLMSG_Message_t
	{
		struct list_head list;
		SL_U32 cmd;
		SL_U32 param;
		SL_POINTER data;
	}SLMSG_Message_t;

    SL_ErrorCode_t SLMSG_Init (void);

    SL_ErrorCode_t SLMSG_Term (void);

	SL_ErrorCode_t SLMSG_Open (SL_Handle_t* Handle);
	SL_ErrorCode_t SLMSG_Close (SL_Handle_t Handle);

	SL_ErrorCode_t SLMSG_WaitMessage(SL_Handle_t Handle, SLMSG_Message_t* Message, SL_S32 Miliseconds);
	SL_ErrorCode_t SLMSG_PollMessage(SL_Handle_t Handle, SLMSG_Message_t* Message);

	SL_ErrorCode_t SLMSG_PostMessage(SL_Handle_t Handle, SLMSG_Message_t* Message);

#ifdef __cplusplus
}
#endif

#endif
