#include "sharemem.h"


extern char web_flag;
extern char multicast[20];


int InitShareMem(void)
{
	int shmid;
	int i; 
	
	shmid = shmget((key_t)1234, sizeof(SHARE_MEM), 0666|IPC_CREAT);	
	if(shmid == -1)  
	{  
		printf("shmget failed\n");  
		exit(0);  
	} 

	share_mem = (struct shared_use_st*)shmat(shmid, (void*)0, 0);  
	if(share_mem == NULL) 
	{  
		printf("shmat failed\n");
		exit(0);
	}
	printf("Memory attached at %X\n", (int)share_mem);  

    //share_mem->sm_eth_setting = eth_setting;
    //share_mem->sm_wlan_setting = wlan_setting;
    AppInitCfgInfoDefault();
    
#if 0

	share_mem->sm_group_pack = group_pack;
    for (i=0; i<128; i++)
    {
		//share_mem->sm_group_pack.ucMultiAddress[i] = group_pack.ucMultiAddress[i];
		//share_mem->sm_group_rename.rxRename[i][0] = group_rename.rxRename[i][0];
		//printf("multicast : %d \n", group_pack.ucMultiAddress[i]);
		//printf("rxRename : %s \n", group_rename.rxRename[i][0];);
	}
	for (i=0; i<24; i++)
	{
		share_mem->sm_group_rename.txRename[i][0] = group_rename.txRename[i][0];
	}
#endif

    //printf("%s \n",eth_setting.strEthIp);
    //printf("%s \n",wlan_setting.strWlanIp);

    //printf("%s \n",share_mem->sm_eth_setting.strEthIp);
    //printf("%s \n",share_mem->sm_wlan_setting.strWlanIp);        
}

int UpdateShareMemStatus(void)
{
}

#if 1
SL_POINTER  sharemem_handle(SL_POINTER Args)
{
	int ret;
	int fd_config;
	
	while(1)
	{
		#if 0
		//printf("ucUpdateFlag=%d\n",share_mem->ucUpdateFlag);
		if(1==share_mem->ucUpdateFlag)
		{
			printf("\n\n--------web start -------\n");
			AppWriteCfgInfotoFile();
#if 1
			//strcpy(multicast, share_mem->sm_eth_setting.strEthMulticast);
#endif
			//SLVENC_setBitrate(share_mem->sm_encoder_setting.usEncRate);
			web_flag = 1;
			//share_mem->ucUpdateFlag = 0;
			//printf("\n\n--------------web end \n");
		}
		if (1==share_mem->ucModeApplyFlag)
		{
			printf("\n\n--------mode apply -------\n");
			AppWrinteModeInfotoFile(); //mode 
			ret = AppInitCfgInfoFromFile(&fd_config); //flush current share memory
			if (ret < 0)
			{
				if (NULL!=fd_config)
					close(fd_config);
				printf("share memary cfg Info from file error \n");
			}
			//share_mem->ucModeApplyFlag = 0;
		}
		#endif
		sleep(1);
	}
	return NULL;
}

#else

SL_POINTER  sharemem_handle(SL_POINTER Args)
{
    while(1)
    {
        if(1==share_mem->ucUpdateFlag)
        {
            eth_setting = share_mem->sm_eth_setting;
            share_mem->ucUpdateFlag = 0;
            AppWriteCfgInfotoFile();               
        }
        sleep(1);
    }


    return NULL;
}
#endif
