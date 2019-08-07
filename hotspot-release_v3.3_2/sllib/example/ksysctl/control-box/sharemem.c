#include "sharemem.h"
#include "config.h"


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

    AppInitCfgInfoDefault();
}


SL_POINTER  sharemem_main(SL_POINTER Args)
{
	int ret;
	int fd_config;
	
	while(1)
	{
		//printf("ucUpdateFlag=%d\n",share_mem->ucUpdateFlag);
		if(ON == share_mem->ucUpdateFlag)
		{
			printf("\n\n--------web start -------\n");
			AppWriteCfgInfotoFile();  

			share_mem->ucUpdateFlag = 0;
		}
		sleep(1);
	}
	return NULL;
}

