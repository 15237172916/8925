/**    *********************************************************************
#**
#**                            sl_param_parser.h
#**                        Silan Soft Project
#**
#**       (c)Copyright 2010-2014, HangZhou Silan Microelectronics Co.,Ltd
#**                        All Rights Reserved 
#**                       http://www.silan.com
#**
#**    File Name    : sl_param_parser.h
#**    Description  : Commonly used functions to calculate video related parameters
#**    Modification : 2014/05/20      xuhang      First created
#**
#**    *********************************************************************
*/

#ifndef _SL_PARAM_PARSER_H
#define _SL_PARAM_PARSER_H

#include <sl_types.h>
#include <sl_param_sys.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 PARAMPARSE_pixelFormat(const char *option, const char *value,
        SL_U16 *chan_id, pixelFormat_e *format);

SL_S32 PARAMPARSE_resoRatio(const char *option, const char *value,
        SL_U16 *chan_id, resoRatio_e *reso);

SL_S32 PARAMPARSE_frameNum(const char *option, const char *value,
        SL_U16 *chan_id, SL_S32 *frame_num);

SL_S32 PARAMPARSE_resoRatioWidth(const char *option, const char *value,
        SL_U16 *chan_id, SL_S32 *reso_wi);

SL_S32 PARAMPARSE_resoRatioHeight(const char *option, const char *value,
        SL_U16 *chan_id, SL_S32 *reso_hi);

SL_S32 PARAMPARSE_inputFile(const char *option, const char *value,
        SL_U16 *chan_id, const char **fname);

SL_S32 PARAMPARSE_outputFile(const char *option, const char *value,
        SL_U16 *chan_id, const char **fname);

SL_S32 PARAMPARSE_configFile(const char *option, const char *value,
        SL_U16 *chan_id, const char **fname);

SL_S32 PARAMPARSE_scale(const char *option, const char *value,
        SL_U16 *chan_id, double *scale);

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_PARSER_H */
