#ifndef __SL_PARSE_FILE_H  
#define __SL_PARSE_FILE_H  

#ifdef __cplusplus  
extern "C" {  
#endif  

#include <stdio.h>
#include <sl_param_video.h>

typedef enum __nodeType
{
    NODE_TYPE_GLOBAL,
    NODE_TYPE_DEV
}nodeType_e;

typedef struct __channelInfo
{
    int channelid;
    pixelFormat_e format;
    int width;
    int height;    
    struct __channelInfo *pNext;
}channelInfo_s;

typedef struct __devCfgInfo
{
    nodeType_e type;
    devId_e devId;
    char dev_name[16];
    channelInfo_s *pChannelInfo;
    struct __devCfgInfo *pNext;
}devCfgInfo_s;
 
typedef enum __parse_mode
{
    PARSE_MODE_CMD,
    PARSE_MODE_FILE
}parseMode_e;

typedef struct __parse_ctx
{
    parseMode_e parse_mode;
    char filename[128];
    int line_num;
    FILE *fpConfig; 
    char *cmdBuff;
    devCfgInfo_s *pHeadDev;
}parseCtx_s;

parseCtx_s *SL_parseInit(parseMode_e parse_mode, char *pInFileName);
int SL_parseDeinit(parseCtx_s *pCtx);
int SL_parseCfg(parseCtx_s *pCtx);  

void SL_parseCfgPrint(devCfgInfo_s *pHead);
channelInfo_s *SL_getGlobalCfg(devCfgInfo_s *pHead, int channleNum);
channelInfo_s *SL_findChannelByDevIdChNum(devCfgInfo_s *pHead, devId_e devId, int channleNum);
#ifdef __cplusplus  
}  
#endif  
  
#endif  
