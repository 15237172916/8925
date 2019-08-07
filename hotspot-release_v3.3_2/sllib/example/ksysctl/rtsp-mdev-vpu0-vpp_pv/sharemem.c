#include "sharemem.h"
#include <stdbool.h>
extern bool g_multicastChangeFlag;
extern char g_multicast[20];
extern char g_serverip[20];

int InitShareMem(void)
{
	int shmid;
	printf("statr init share memory\n");
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
#if 0
    //share_mem->sm_eth_setting = eth_setting;
    //share_mem->sm_wlan_setting = wlan_setting;
    
    printf("%s \n",eth_setting.strEthIp);
    printf("%s \n",wlan_setting.strWlanIp);        

    printf("%s \n",share_mem->sm_eth_setting.strEthIp);
    printf("%s \n",share_mem->sm_wlan_setting.strWlanIp);        
#endif
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
		if (1==share_mem->ucUpdateFlag)
		{
			printf("web start \n");
			AppWriteCfgInfotoFile();
#if 0
			strcpy(g_multicast, share_mem->sm_eth_setting.strEthMulticast); //get now multicast address
			//init_eth(); //set ip address
#endif
			if (ON == share_mem->sm_rx_info.tv_status)
			{
				//send tv on signal

			}
			else
			{
				//send tv off signal

			}
			
			g_multicastChangeFlag = true;
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
