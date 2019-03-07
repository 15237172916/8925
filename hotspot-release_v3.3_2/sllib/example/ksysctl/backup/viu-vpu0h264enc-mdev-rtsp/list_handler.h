#ifndef _LIST_HANDLER_H
#define _LIST_HANDLER_H

#define  MAX_NODES 50

typedef struct _node_data {
    void*  data;
    struct _node_data *pnext;
}NODE_DATA_S;

typedef  struct _list_buffer {
	NODE_DATA_S *push;
	NODE_DATA_S *fetch;	
	void *mutexlock;
	SL_U32 nodes_num;
}LIST_BUFFER_S ;

int list_push_data(LIST_BUFFER_S *list, void* phy_addr);
void *list_flush_data(LIST_BUFFER_S *list);
int list_fetch_data(LIST_BUFFER_S *list, void **phy_addr);
int list_release_data(LIST_BUFFER_S *list);
LIST_BUFFER_S * list_init(void);
void list_uninit(LIST_BUFFER_S *list);
#endif

