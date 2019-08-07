#include <stdbool.h>
#include "sharemem.h"


extern bool g_multicastChangeFlag;
extern char g_strMulticast[20];
extern SL_S32 SLVENC_setBitrate(SL_U32 bitRate);

int InitShareMem(void)
{
	int shmid;
	int i; 
	
	shmid = shmget((key_t)1234, sizeof(SHARE_MEM_S), 0666|IPC_CREAT);	
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
    
  	AppInitCfgInfoDefault();
    
	return 0;
}


SL_POINTER  sharemem_handle(SL_POINTER Args)
{
	int ret;
	int fd_config;
	
	while(1)
	{
		#if 1
		//printf("ucUpdateFlag=%d\n",share_mem->ucUpdateFlag);
		if(1==share_mem->ucUpdateFlag)
		{
			printf("\n\n--------web start -------\n");
			AppWriteCfgInfotoFile();
#if 1
			strcpy(g_strMulticast, share_mem->sm_eth_setting.strEthMulticast);
#endif
			g_multicastChangeFlag = true;
			share_mem->ucUpdateFlag = 0;
		}
		#endif
		
		sleep(1);
	}
	return NULL;
}


