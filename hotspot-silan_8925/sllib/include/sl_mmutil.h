/**    *********************************************************************
#**
#**                            sl_mmutil.h
#**                        Silan Soft Project
#**
#**       (c)Copyright 2010-2014, HangZhou Silan Microelectronics Co.,Ltd
#**                        All Rights Reserved 
#**                       http://www.silan.com
#**
#**    File Name    : sl_mmutil.h
#**    Description  : Commonly used utility functions to process parameters
#**    Modification : 2014/12/29      xuhang      First created
#**
#**    *********************************************************************
*/

#ifndef _SL_MMUTIL_H
#define _SL_MMUTIL_H

#include <sl_types.h>
#include <sl_param_sys.h>
#include <sl_param_video.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MMUTIL_getFrameSize() - Get size of a frame according to parameters of the
 *                  video
 *
 * @param vp: Parameters of the video
 * @param size: Size of a frame
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_getFrameSize(videoPara_s *vp, SL_U32 *size);

/**
 * MMUTIL_resoRatioId2Size() - Convert resolution ration from ID to size
 *
 * @param reso: Resolution ration in type of resoRatio_e
 * @param wi: Width of the resolution
 * @param hi: Height of the resolution
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_resoRatioId2Size(resoRatio_e reso, SL_U16 *wi, SL_U16 *hi);

/**
 * MMUTIL_resoRatioStr2Id() - Convert resolution ratio from string to ID
 *
 * @param str: String of the resolution ratio
 * @param reso: ID of the resolution ratio
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_resoRatioStr2Id(const char *str, resoRatio_e *reso);

/**
 * MMUTIL_calcuResoRatioSize() - Calculate size of resolution ratio
 *
 * @param cvp: Video parameters array of channel(s)
 * @param size_cvp: Size of the array
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_calcuResoRatioSize(chanVideoPara_s *cvp, SL_U32 size_cvp);

/**
 * MMUTIL_pixelFormatStr2Id() - Convert pixel format from string to ID
 *
 * @param str: String of the pixel format
 * @param format: ID of the pixel format
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_pixelFormatStr2Id(const char *str, pixelFormat_e *format);

/**
 * MMUTIL_devNameStr2Id() - Convert device name from string to ID
 *
 * @param str: Device name
 * @param id: ID of the device
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_devNameStr2Id(const char *str, devId_e *id);

/**
 * MMUTIL_encodeMsg() - Encode a message
 *
 * @param head: Head of the parameter message
 * @param para_id: Parameter ID
 * @param size: Size of the parameters
 * @param para: The parameters
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_encodeMsg(paraHead_s *head, SL_U32 para_id, SL_U32 size, void *para);

/**
 * MMUTIL_decodeMsg() - Decode a message
 *
 * @param head: Head of the parameter message
 * @param para_id: Parameter ID
 * @param size: Size of the parameters
 * @param para: The parameters
 *
 * @return SL_NO_ERROR: OK
 *         !SL_NO_ERROR: ERROR
 */
SL_S32 MMUTIL_decodeMsg(paraHead_s *head, SL_U32 *para_id, SL_U32 *size,
        void **para);

#ifdef __cplusplus
}
#endif

#endif /* _SL_MMUTIL_H */
