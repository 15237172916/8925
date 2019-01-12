/*************************************************************************
	> File Name: rtp.c
	> Author: 
	> Mail: 
	> Created Time: Sat 23 Jun 2018 05:02:39 PM CST
 ************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>                    
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>


#include "rtp.h"


FILE *fp_outfile;

int output(void)
{
	
	char outfilename[12] = "./test.264";
	
	if ((fp_outfile = fopen(outfilename, "wb")) == NULL)
	{
		printf("can't open %s \n", outfilename);
		return -1;
	}
	printf("open %s file ok \n", outfilename);
	
	return 0;
}


int main(int argc, char* argv[])
{
    int ret, i;
    void *p_buf = NULL;
    ssize_t len = 0;
    char buf[BUFSIZE];
    //char * pbuf = malloc(100*sizeof(char));

    int sock_client;
    struct sockaddr_in server_addr;
    
    /*
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SIGNAL_CLIENT_PORT);
    servaddr.sin_addr.s_addr = inet_addr(serverip);
    */
    
    p_buf = malloc(sizeof(char)*BUFSIZE);
    output();
    
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family=AF_INET;  
    server_addr.sin_port=htons(DEST_PORT);            
    server_addr.sin_addr.s_addr=inet_addr(DEST_IP);   
    
    
ReSocket:
    sock_client = socket(AF_INET,SOCK_STREAM, 0);
	if (sock_client < 0)
    {
        printf("signal Create Socket Failed!\n");
        perror("socket");
        //close(sock_client);
        sleep(1);
        goto ReSocket;
    }
    
ReConnect:
    printf("--------------signal Connection start----------------------\n");
    ret = connect(sock_client, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        printf("signal channel connect error, ret = %d \n",ret);
        printf("errno is %d\n",errno);
        sleep(1);
        goto ReConnect;
    }

	while(1)
	{
		len = recv(sock_client, buf, BUFSIZE, 0);
        if(len <= 0)
        {
            printf("Recv heart len < 0 \n");
            close(sock_client);
            exit(0);
            sleep(1);
            goto ReSocket;
        }
        else
        {
			if(fwrite(buf, len, 1, fp_outfile))
			{
				printf("fwrite error\n");
			}
			//tmp += len;
			for (i=0; i < len; i++)
			
				printf(" %x ", buf[i]);
        }
	}
	fclose(fp_outfile);
	close(sock_client);
	return 0;
}



