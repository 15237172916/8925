#ifndef _SL_PARAM_H264ENC_H
#define _SL_PARAM_H264ENC_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef SL_U32 SLH264ENC_Handle_t;

typedef enum _H264_EntropyMode
{
    CABAC_MODE,
    UVLC_MODE,

}
SLH264ENC_EntropyMode_e;

typedef enum _H264ENC_GetStream_stat
{
    GET_ONESTREAM,
    GET_DOUBSTREAM,
}
SLH264ENC_GetStream_stat_e;

enum IP_TYPE_e
{
    IP_TYPE_I = 0,
    IP_TYPE_P,
};

enum SUB_IP_TYPE_e
{
    SUB_IP_TYPE_I = 0,
    SUB_IP_TYPE_P,
};

typedef struct _SLH264ENC_ExtendStream_info_s
{
    SL_U32 stream_offset_addr;           //main stream addr
    SL_S32 stream_size;
    SL_U32 sub_stream_offset_addr;
    SL_S32 sub_stream_size;
    enum IP_TYPE_e IP_type;
    enum SUB_IP_TYPE_e SUB_IP_type;

}SLH264ENC_ExtendStream_info_s;

typedef struct  _SLH264ENC_Encode_s
{
    SL_U32 chnId;
    SLH264ENC_EntropyMode_e entropyMode;
    SL_U32 srcFrameRate;
    SL_U32 frameRate;
    SL_U32 width;
    SL_U32 height;
    SL_U32 ctxNum;
    SL_U32 qp;
    SL_U32 rate_i_p; 
    SL_U32 rate_IDR; 
    SL_U32 SLRCEnable;
    SL_U32 bit_rate;

} SLH264ENC_Encode_s;

#ifdef __cplusplus
}
#endif

#endif /* _SL_PARAM_H264ENC_H */
