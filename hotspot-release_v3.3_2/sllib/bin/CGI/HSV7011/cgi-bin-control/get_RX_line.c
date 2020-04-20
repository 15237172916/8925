#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Setting.h"
#include "sharemem.h"

int InitShareMem(void)
{
	int shmid;

	shmid = shmget((key_t)1234, sizeof(SHARE_MEM), 0666|IPC_CREAT);	
	if(shmid == -1)  
	{  
		printf("shmget failed\n");  
		exit(0);  
	}

	share_mem = shmat(shmid, (void*)0, 0);  
	if(share_mem == NULL)  
	{  
		printf("shmat failed\n");	
		exit(0);  
	}
	//printf("Memory attached at %X\n", (int)share_mem);  
    //share_mem->sm_eth_setting = eth_setting;
}
#if 1
char* get_cgi_data(FILE* fp, char* method)
{
    char* input;
    int len;
    int size=1024*10;
    int i=0;

    if (strcmp(method, "GET") == 0)  /**< GET method */
    {
        input = getenv("QUERY_STRING");
        return input;
    }
    else if (strcmp(method, "POST") == 0)  /**< POST method */
    {
        len = atoi(getenv("CONTENT_LENGTH"));
        //printf("size:-%d-\n",len);
        input = (char*)malloc(sizeof(char) * (size+1));

        if (len == 0)
        {
            input[0] = '\0';
            return input;
        }

        while (1)
        {
            input[i] = (char)fgetc(fp);
            //printf("-%c-\n",input[i]);
            if (i == size)
            {
                //printf("1");
                input[i+1] = '\0';
                return input;
            }
            --len;

            if (feof(fp) || (!(len)))
            {
                i++;
                //printf("2");
                input[i] = '\0';
                return input;
            }
            i++;
        }
    }
    return NULL;
}
#endif
void main(void)
{
    char *data;
    FILE *fp;
	char * type;
    char * method;
	unsigned i;
	unsigned char str[204800];
	unsigned char strTemp[100];
	int j=1;
    printf("Content-Type:text/html\n\n");
    type = getenv("CONTENT_TYPE");
    method = getenv("REQUEST_METHOD");
    data = get_cgi_data(stdin, method); 
    if(NULL==data)
    {
        printf("Parameter Error \n");
        exit(0);
    }
	
	 InitShareMem();
	strcpy(str,"<set>");
	share_mem->ucOnlineNum[200]=1;
	for(i=0;i<128;i++)
	{
		sprintf(strTemp,"<RX%d>",(i+1));
		strcat(str,strTemp);
		sprintf(strTemp,"%d",share_mem->ucOnlineNum[i]);
		//sprintf(strTemp,"%d",share_mem->rx_info[i].status);
		strcat(str,strTemp);
		sprintf(strTemp,"</RX%d>",(i+1));
		strcat(str,strTemp);
	}
	for(i=200;i<224;i++)
	{
		//printf("%d\n",share_mem->tx_info[0].status);
	
		 sprintf(strTemp,"<TX%d>",(i+1));
		 strcat(str,strTemp);
		sprintf(strTemp,"%d",share_mem->ucOnlineNum[i]);
		// sprintf(strTemp,"%d",share_mem->tx_info[i].status);
	
		 strcat(str,strTemp);
		 sprintf(strTemp,"</TX%d>",(i+1));
		 strcat(str,strTemp);
	}
	strcat(str,"</set>");
	printf("%s\n",str);
	
}

