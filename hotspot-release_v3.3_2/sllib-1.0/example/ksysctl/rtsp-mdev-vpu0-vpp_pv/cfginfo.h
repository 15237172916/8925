#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

typedef struct cfginfo
{
    char ip[32];
    char serverip[32];
    int port;
    char flag;
}CFG_INFO_S;

int InitCfgInfo(int *fp);
int update_cfg_info(int fpConfig, CFG_INFO_S *pcfginfo);
CFG_INFO_S *GetCfgInfo();

int AppInitCfgInfoDefault(void);
int AppInitCfgInfoFromFile(int *fp);
int AppWriteCfgInfotoFile(void);

#endif
