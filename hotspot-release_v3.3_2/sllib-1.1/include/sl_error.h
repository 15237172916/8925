/*
 * Error code definition
 */
#ifndef _SL_ERROR_H
#define _SL_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error codes
 */
#define SL_NO_ERROR                     0
#define SL_ERROR_BAD_PARAMETER          1
#define SL_ERROR_NOT_IMPLEMENTED        2
#define SL_ERROR_NO_MEMORY              3
#define SL_ERROR_UNKNOWN_DEVICE         4
#define SL_ERROR_ALREADY_INITIALIZED    5
#define SL_ERROR_NOT_INITIALIZED        6
#define SL_ERROR_NO_FREE_HANDLE         7
#define SL_ERROR_OPEN_HANDLE            8
#define SL_ERROR_INVALID_HANDLE         9
#define SL_ERROR_FEATURE_NOT_SUPPORT    10
#define SL_ERROR_TIMEOUT                11
#define SL_ERROR_DEVICE_BUSY            12
#define SL_ERROR_GENERIC_IO             13
#define SL_ERROR_NOT_FOUND              14
#define SL_ERROR_UNDEFINED              15
#define SL_ERROR_UNDERFLOW              16
#define SL_ERROR_OVERFLOW               17
#define SL_ERROR_HARDWARE               18
#define SL_ERROR_NO_MORE                19
#define SL_ERROR_VERSION_MISMATCH       20
#define SL_ERROR_NOT_READY              21
#define SL_ERROR_INVALID_MODE           22
#define SL_ERROR_UNSUPPORT_FORMAT       23
#define SL_ERROR_MAX                    24

/*
 * Error checking
 */
#define SL_SUCCESS(err) (SL_NO_ERROR == err)
#define SL_ERROR(err) (SL_NO_ERROR != err)

#ifdef __cplusplus
}
#endif

#endif /* _SL_ERROR_H */
