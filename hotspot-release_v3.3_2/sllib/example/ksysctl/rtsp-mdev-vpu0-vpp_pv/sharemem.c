#include "sharemem.h"

extern char web_flag;
extern char multicast[20];
extern char serverip[20];

int InitShareMem(void)
{
	int shmid;
	printf("statr init share memory\n");
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
    
    printf("%s \n",eth_setting.strEthIp);
    printf("%s \n",wlan_setting.strWlanIp);        

    printf("%s \n",share_mem->sm_eth_setting.strEthIp);
    printf("%s \n",share_mem->sm_wlan_setting.strWlanIp);        
}

int UpdateShareMemStatus(void)
{
}

#if 1
SL_POINTER  sharemem_handle(SL_POINTER Args)
{
	int ret;
	
	while(1)
	{
		//printf("ucUpdateFlag=%d\n",share_mem->ucUpdateFlag);
		if(1==share_mem->ucUpdateFlag)
		{
			printf("web start \n");
			AppWriteCfgInfotoFile();
#if 1
			strcpy(multicast, share_mem->sm_eth_setting.strEthMulticast);
#endif	
			web_flag = 1;
			share_mem->ucUpdateFlag = 0;
			printf("web end \n");
		}
		usleep(500000); //0.5s
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
