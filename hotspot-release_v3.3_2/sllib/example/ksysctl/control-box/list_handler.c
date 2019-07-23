#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sl_os.h>
#include <list_handler.h>

int list_push_data(LIST_BUFFER_S *list, void* phy_buf)
{
	NODE_DATA_S *ptmp;

	ptmp = malloc(sizeof(NODE_DATA_S));
	if (NULL == ptmp) {
		return -1;
	}
	ptmp->data = phy_buf;
	ptmp->pnext = NULL; 
#if 0
	if (SL_NO_ERROR != SLOS_AcquireMutex(list->mutexlock,SL_INFINITE))
	{
		free(ptmp);
		return -1;
	}
#endif
	if (SL_NO_ERROR != SLOS_AcquireMutex(list->mutexlock,1000)) //1000ms
	{
		log_err("SLOS_AcquireMutex failed");
		reboot1();
	}

	if(MAX_NODES== (list->nodes_num + 1))
	{
		free(ptmp);
		SLOS_ReleaseMutex(list->mutexlock);       
		return 1;
	}
	list->nodes_num ++;


	if (NULL == list->push) {
		list->push = ptmp;
	} else {
		list->push->pnext = ptmp;
		list->push = ptmp;
	}

	if (NULL == list->fetch)
		list->fetch = ptmp;

	SLOS_ReleaseMutex(list->mutexlock);       

	return 0;
}

void *list_flush_data(LIST_BUFFER_S *list)
{
	NODE_DATA_S *ptmp, *phead;
	int cnt = 0;

	//printf("%s\n",__func__);
	//SLOS_AcquireMutex(list->mutexlock,SL_INFINITE); 
	if (SL_NO_ERROR != SLOS_AcquireMutex(list->mutexlock,1000)) //1000ms
	{
		log_err("SLOS_AcquireMutex failed");
		reboot1();
	}

	phead = list->fetch;
	list->fetch = list->push = NULL;

	while(phead)
	{
		ptmp = phead;
		phead = phead->pnext;
		free(ptmp);
		cnt ++;
		if(cnt > MAX_NODES) 
		{
			printf("cnt > MAX_NODES\n");
			log_err("%d reboot",__LINE__);
			reboot1();
		}
	}

	list->nodes_num = 0;
	SLOS_ReleaseMutex(list->mutexlock);
	return 0;
}

int list_fetch_data(LIST_BUFFER_S *list, void **phy_addr)
{
	NODE_DATA_S *ptmp;
	if (SL_NO_ERROR != SLOS_AcquireMutex(list->mutexlock,1000)) //1000ms
	{
		log_err("SLOS_AcquireMutex failed");
		reboot1();
	}
#if 0
	if (SL_NO_ERROR != SLOS_AcquireMutex(list->mutexlock,SL_INFINITE))
	{
		return -1;
	} 
#endif
	if (NULL == list->fetch)
	{ 
		SLOS_ReleaseMutex(list->mutexlock);
		return -1;
	}

	ptmp = list->fetch;

	*phy_addr = ptmp->data;

	list->fetch = ptmp->pnext;

	if (NULL == list->fetch)
		list->push = NULL;

	free(ptmp);   

	list->nodes_num --;
	SLOS_ReleaseMutex(list->mutexlock);    

	return 0;
}

int list_release_data(LIST_BUFFER_S *list)
{
	NODE_DATA_S *ptmp;

	ptmp = list->fetch;

	list->fetch = ptmp->pnext;

	if (NULL == list->fetch)
		list->push = NULL;

	free(ptmp);   

	SLOS_ReleaseMutex(list->mutexlock);    

	return 0;
}

LIST_BUFFER_S * list_init(void)
{
	LIST_BUFFER_S *list;
	list = malloc(sizeof(LIST_BUFFER_S));
	if(!list)
		return NULL;

	memset(list,0x00,sizeof(LIST_BUFFER_S));

	SLOS_CreateMutex(&list->mutexlock);  
	list->nodes_num  = 0;

	return list;
}

void list_uninit(LIST_BUFFER_S *list)
{
	SLOS_DeleteMutex(list->mutexlock);        

	free(list);
}
