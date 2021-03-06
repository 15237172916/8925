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

void main(void)
{
    char str[10240];
    char strTemp[40];
    unsigned char i;
    
    //printf("init share memary \n");
    InitShareMem();
	
    strcpy(str,"<set>");
	
#if 1
    for (i=0; i<128; i++)
    {
		sprintf(strTemp,"<RX%d>", (i+1));
		strcat(str, strTemp);
		sprintf(strTemp,"%s", share_mem->sm_group_rename.rxRename[i]);
		strcat(str, strTemp);
		sprintf(strTemp, "</RX%d>", (i+1));
		strcat(str, strTemp);
	}
	for (i=0; i<128; i++)
    {
		sprintf(strTemp,"<MUL%d>", (i+1));
		strcat(str, strTemp);
		sprintf(strTemp,"%d", share_mem->sm_group_pack.ucMultiAddress[i]);
		strcat(str, strTemp);
		sprintf(strTemp, "</MUL%d>", (i+1));
		strcat(str, strTemp);
	}
	for (i=0; i<24; i++)
	{
		sprintf(strTemp,"<TX%d>", (i+1));
		strcat(str, strTemp);
		sprintf(strTemp,"%s", share_mem->sm_group_rename.txRename[i]);
		strcat(str, strTemp);
		sprintf(strTemp, "</TX%d>", (i+1));
		strcat(str, strTemp);
	}
	for (i=0; i<10; i++)
	{
		sprintf(strTemp,"<MODE%d>", (i+1));
		strcat(str, strTemp);
		sprintf(strTemp,"%s", share_mem->sm_mode_rename.modeRename[i]);
		strcat(str, strTemp);
		sprintf(strTemp, "</MODE%d>", (i+1));
		strcat(str, strTemp);
	}
	
	sprintf(strTemp,"<CurrentMode>");
	strcat(str, strTemp);
	sprintf(strTemp,"%d", share_mem->ucCurrentMode);
	strcat(str, strTemp);
	sprintf(strTemp, "</CurrentMode>");
	strcat(str, strTemp);
#endif
    strcat(str,"</set>");
    
	printf("content-Type:text/xml\n\n");
	printf("%s\n",str);
	
}
