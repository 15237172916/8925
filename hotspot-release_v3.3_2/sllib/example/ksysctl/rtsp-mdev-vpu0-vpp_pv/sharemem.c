#include "sharemem.h"
#include "EEPROM.h"
#include "../version.h"

extern char web_flag;
extern char multicast[20];
extern char serverip[20];
extern char g_Is_E2prom;

static int fd_config;

int InitShareMem(void)
{
	int shmid;
	int ret = -1;

	printf("statr init share memory\n");
	shmid = shmget((key_t)1234, sizeof(SHARE_MEM), 0666|IPC_CREAT);	
	if (shmid == -1)
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

	AppInitCfgInfoDefault(); //init share_mem to define data
#ifdef CONFIG_EEPROM
	ret = InitShareMemFromE2prom(share_mem); //check eeprom 
#else
	ret = -1;
#endif
	printf("ret : %d \n", ret);
	if (ret < 0) //isn't eeprom
	{
		AppInitCfgInfoDefault();
		g_Is_E2prom = 0;
		//printf("\n\n*****e2prom read error*****\n\n");
		ret = AppInitCfgInfoFromFile(&fd_config); //reinit share memory form file
		if (ret < 0) 
		{
			if(NULL!=fd_config)
				close(fd_config);
			printf("build default config.conf \n");
			AppWriteCfgInfotoFile();
		}
		else
		{
			printf("cfg get from file \n");
			close(fd_config);
		}
	}
	else
	{
		g_Is_E2prom = 1;

		if (2 == ret)
		{
			AppInitCfgInfoDefault();
			WriteConfigIntoE2prom(share_mem);
		}
	}
	printf("share_mem->sm_eth_setting.strEthIp : %s \n", share_mem->sm_eth_setting.strEthIp);
	printf("share_mem->sm_eth_setting.strEthMulticast : %s \n", share_mem->sm_eth_setting.strEthMulticast);
	printf("Memory attached at %X\n", (int)share_mem);
	sleep(2);
}

void sharemem_handle(void)
{
	int ret;
	share_mem->ucUpdateFlag = 0;
	while(1)
	{
		//printf("ucUpdateFlag=%d\n",share_mem->ucUpdateFlag);
		if (1==share_mem->ucUpdateFlag)
		{
			printf("\nweb start \n");
			strcpy(multicast, share_mem->sm_eth_setting.strEthMulticast); //get now multicast address
			web_flag = 1;
			//AppWriteCfgInfotoFile();
			share_mem->ucUpdateFlag = 0;
			printf("\n\nweb end \n\n");
		}
		usleep(100000); //0.1s
	}
	return NULL;
}
