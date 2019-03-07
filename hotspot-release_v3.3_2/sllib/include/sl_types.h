/*
 * Basic types definition
 */
#ifndef _SL_TYPES_H
#define _SL_TYPES_H

/*
 * Header files
 */
#include <sl_debug.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Basic data type
 */
typedef signed char   SL_S8;
typedef unsigned char SL_U8;

typedef signed short   SL_S16;
typedef unsigned short SL_U16;

typedef signed int     SL_S32;
typedef unsigned int   SL_U32;

typedef unsigned long long   SL_U64;
typedef signed long long   SL_S64;

typedef float		   SL_FLOAT;
typedef double		   SL_DOUBLE;

typedef char			SL_CHAR;

typedef void*		   SL_POINTER;
typedef char*		   SL_STRING;

typedef char* SL_String_t;
typedef SL_U32 SL_Handle_t;

typedef enum _SL_BOOL {
    SL_FALSE = 0,
    SL_TRUE = 1
} SL_BOOL;

/* 
 * Aliases of basic data type
 */
#ifndef __KERNEL__
typedef SL_U8  u8;
typedef SL_U16 u16;
typedef SL_U32 u32;
typedef SL_U64 u64;
typedef SL_S8  s8;
typedef SL_S16 s16;
typedef SL_S32 s32;
typedef SL_S64 s64;
#endif /* __KERNEL__ */

/*
 * Error code
 */
typedef SL_S32 SL_ErrorCode_t;

/*
 * Strings
 */
#define SL_MAX_DEVICE_NAME  20
typedef char SL_ModuleName_t[SL_MAX_DEVICE_NAME];
typedef const char *SL_Revision_t;

/*
 * Consts
 */
#define SL_NULL             0
#define SL_INFINITE         0
#define SL_INVALID_HANDLE   (~0)

#ifdef __cplusplus
}
#endif

#endif /* _SL_TYPES_H */
