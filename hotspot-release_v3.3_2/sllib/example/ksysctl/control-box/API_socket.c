#include <sys/types.h>
#include <sys/socket.h>                        
#include <stdio.h>
#include <netinet/in.h>                        
#include <arpa/inet.h>                     
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <errno.h>


#define API_PORT  23
#define BUFFER_SIZE 1024
#define LENGTH_OF_LISTEN_QUEUE 20

void *API_server(void)
{
	//sockfd
	int server_socket, new_server_socket;
	int len;
    //set socket's address information
    struct sockaddr_in   server_addr;
   
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    //server_addr.sin_port = htons((uint16_t)(port));
    server_addr.sin_port = API_PORT; 

Resocket:   
    // create a stream socket
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        printf("Create Data Socket Failed!\n");
        perror("socket");
        goto Resocket;
    }
    printf("Create  Socket OK\n");

    //bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Data Bind Port: %d Failed!\n", API_PORT);
        perror("bind");
        close(server_socket);
        sleep(1);
        goto Resocket;
    }
    
    // listen
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        printf("Server Data Listen Failed!\n");
        perror("listen");
        close(server_socket);
        sleep(1);
        goto Resocket;
    }
    printf("Data listen socket ok \n");
        
ReAccept:

	printf("Data Start Accept \n");
	new_server_socket = accept(server_socket, NULL,NULL);
	if (new_server_socket < 0)
	{
		printf("Server Data Accept Failed!\n");
		perror("accept");
		printf("accept errno : %d \n", errno);
        close(server_socket);
        sleep(1);
        goto Resocket;
	}
	printf("Server Data Aceept OK \n");
	
    unsigned char*buf = NULL;
    
	buf = (unsigned char*)malloc(sizeof(unsigned char)*BUFFER_SIZE);
	
    while(1)
    {		

        len = recv(new_server_socket, buf, BUFFER_SIZE,0);
        if (len <= 0)
        {
            printf("data recv error \n");
            goto ReAccept; 
        }
        else
        {
            printf("data recv = %d \n", len);
        }

        len = send(new_server_socket, buf, BUFFER_SIZE,0);
        if(len <= 0)
        {
            printf("Send data len <= 0 len= %d \n",len);
            perror("send");
            printf("send errno : %d \n", errno);
            
            close(new_server_socket);
            goto ReAccept;
        }
    }

	free(buf);
    close(new_server_socket);

    return 0;
}

