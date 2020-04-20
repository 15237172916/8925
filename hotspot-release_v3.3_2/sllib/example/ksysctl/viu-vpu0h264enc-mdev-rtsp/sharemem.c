#include <stdlib.h>

#include "sharemem.h"
#include "main.h"

extern char web_flag;
extern char multicast[20];
extern SL_S32 SLVENC_setBitrate(SL_U32 bitRate);
extern char g_backup_flag;

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

    share_mem->sm_eth_setting = eth_setting;
    share_mem->sm_wlan_setting = wlan_setting;      
}

void sharemem_handle(void)
{
	int ret;
	int fd_config;
	
	while(1)
	{
		//printf("ucUpdateFlag=%d\n",share_mem->ucUpdateFlag);
		//printf("multicast: %s \n", multicast);
		if(1==share_mem->ucUpdateFlag)
		{
			printf("\n\n--------web start -------\n");
			AppWriteCfgInfotoFile();
#if 1
			strcpy(multicast, share_mem->sm_eth_setting.strEthMulticast);
#endif
			//SLVENC_setBitrate(share_mem->sm_encoder_setting.usEncRate);
			web_flag = 1;
			share_mem->ucUpdateFlag = 0;
			g_backup_flag = 1;
			printf("control flag : %d \n", share_mem->ucControlBoxFlag);
			printf("\n\n--------------web end \n");
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
			share_mem->ucModeApplyFlag = 0;
			
		}
		sleep(1);
	}
	return NULL;
}
