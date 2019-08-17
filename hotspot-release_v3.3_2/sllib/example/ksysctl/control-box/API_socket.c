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
#include "API_socket.h"

int server_socket, new_server_socket;
int cmd_index = -1;
char gCmdFlag;
char cmd[MAX_CMD_PART][MAX_LEN_PART];
unsigned char*buf = NULL;


static void API_send(const void *buf)
{
    printf("sizeof(buf): %d \n", strlen(buf));
    int len = send(new_server_socket, buf, strlen(buf)+1, 0);
    if (len <= 0)
    {
        perror("send");
        printf("Send data len <= 0 len = %d \n",len);
        printf("API send errno : %d \n", errno);
    }
}

static void API_recv(void *buf)
{
    int len = recv(new_server_socket, buf, BUFFER_SIZE, 0);
    if (len <= 0)
    {
        perror("recv");
        printf("recv len <= 0 len = %d \n", len);
        printf("API recv errno : %d \n", errno);
    } 
}

static int cmd_split(char cmd[][MAX_LEN_PART], const char *str)
{
    char *pStrTmp = str;
    int cmd_count = 0;
    int m = 0, n = 0;
    printf("str: %s \n", str);
    while (*pStrTmp != '\0')
    {
        if (*pStrTmp != ' ')
        {
            cmd[m][n] = *pStrTmp;
            n++;
        }
        else
        {
            cmd[m][n] = '\0';
            n = 0;
            m++;
            cmd_count++;
        }
        
        pStrTmp++;
    }
    cmd[m][n] = '\0';
    //printf("cmd[0]: %s, cmd[1]: %s, cmd[2]: %s \n",  cmd[0], cmd[1], cmd[2]);

    return cmd_count;
}



int alias_to_rx_index(const char *alias_rx)
{
    int rx_index = 0;
    
    return rx_index;
}

int alias_to_tx_position(const char *alias_tx)
{
    int tx_position = 0;

    return tx_position;
}

int alias_to_group_index(const char *alias_group)
{
    
}

int alias_to_scene_index(const char *alias_scene)
{

}

int get_tx_position_from_rx(const int rx)
{
    int tx_position = 0;


    return tx_position;
}

int get_rx_index_list_from_tx(const int tx, char *pRx[])
{
    
}

int set_display_group_to_tx(const int display_group, const int tx)
{

}

int set_scene_enable(const int scene_index)
{

}

int set_sink_power(const int power_state, const int *pRx[])
{

}


int set_display_group_power(const int power_state, const int *pGroup[])
{


}

int set_OSD_display(const char OSD_state)
{

}

int get_single_TX_status(const int TX_position)
{

}

int get_single_RX_status(const int RX_index)
{

}

int get_HUB_status(const int HUB)
{

}

int get_single_EXT_status(const int EXT_ID)
{


}

int all_reboot(void)
{

}

int set_notify_state(const int state)
{

}





/*
* function :
*   Switch specified RX units to a specified TX unit.
* commond formate :
*   martrix set <TX> <RX1> <RX2> <RX3> ......
* 
*/
int RX_martrix_command(int tx, int *pRx[])
{

}


static void cmd_parser(char *str)
{
    int cmd_count = 0;
    cmd_count = cmd_split(cmd, str);

    cmd_index = -1;

    printf("cmd count : %d \n", cmd_count);
    //printf("cmd[0] len : %d \n", strlen(cmd[0]));
    if ((0 == strcmp(cmd[0], MATRIX))) //matrix
    {
        printf("MATRIX \n");
        if (cmd_count < 2)
        {
            strcpy(buf, SAMPLE_MATRIX);
            return;
        }
        if ((0 == strcmp(cmd[1], GET))) //matrix get 
        {
            //get RX
            //command : matrix get <RX>
            //response : matrix get <RX> <TX>
            //if ((0 == strcmp(cmd[2], )))

            //get TX
            //command : matrix get <TX>
            //response : matrix get <TX> <RX1> <RX2> ... <RXn>

        }
        else if ((0 == strcmp(cmd[1], SET))) //matrix set
        {
            printf("SET \n");
            //command : matrix set <TX> <RX1> <RX2> .... <RXn>
            //response : matrix set <TX> <RX1> <RX2> .... <RXn>
            
            if (cmd_count < 4)
            {
                strcpy(buf, MATRIX_SET);
                return ;
            }

            
            
        }
        else
        {
           strcpy(buf, UNKNOW); 
           return ;
        }
        
    }
    else if ((0 == strcmp(cmd[0], SCENE))) //scene
    {
        //scene
        //command : scene set <S>
        //response : scene set <S>
        if ((0 == strcmp(cmd[1], SET)))
        {

        }
        else
        {
           strcpy(buf, UNKNOW); 
        } 
    }
    else if ((0 == strcmp(cmd[0], DISPLAY))) //displaygroup
    {
        //displaygroup
        //command : displaygroup set <DG> <TX>
        //response : displaygroup set <DG> <TX>
        if ((0 == strcmp(cmd[1], SET))) //displaygroup set
        {

        }
        else
        {
           strcpy(buf, UNKNOW); 
        } 
    }
    else if ((0 == strcmp(cmd[0], CONFIG))) //config 
    {
        //config 
        //command : config 
        if ((0 == strcmp(cmd[1], GET))) //config get
        {
            //config get TX status <TX>

            //config get RX status <RX>

            //config get HUB status

            //config get EXT status <EXT>

            
        }
        //command : config set
        else if ((0 == strcmp(cmd[1], SET))) //config set
        {
            //config set device
            //command : config set device sinkpower <on|off> <RX1> <RX2> ... <RXn>
            //response : config set device sinkpower <on|off> <RX1> <RX2> ... <RXn> 


            //config set displaygroup
            //command : config set displaygroup sinkpower <on|off> <RX1> <RX2> .... <RXn>
            //response : config set displaygroup sinkpower <on|off> <RX1> <RX2> .... <RXn> 

            //config set OSD
            //command : config set OSD <on|off>
            //response : config set OSD <on|off>

            //config set reboot
            //command : config set reboot
            //response : config set reboot


            //config set notify
            //command : config set notify <on|off>
            //response : config set notify <on|off>
        }
        else
        {
           strcpy(buf, UNKNOW); 
        } 
    }
    /*
    else if ((0 == strcmp(cmd[0], NOTIFY)))
    {
        //notify matrix
        //notification : notify matrix <TX> <RX1> <RX2> ... <Rxn>


        //notify scene
        //notification : notify scene <S> 
        
        //notify 
        
    }
    */
    else 
    {
        strcpy(buf, UNKNOW);
    }



    return;
}


void *API_server(void)
{
	//sockfd
	
	int len, sin_size;
    //set socket's address information
    struct sockaddr_in   server_addr;
    struct sockaddr_in   client_addr;
   
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(API_PORT);

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
    printf("API bind succeed \n");

    //listen
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        printf("Server Data Listen Failed!\n");
        perror("listen");
        close(server_socket);
        sleep(1);
        goto Resocket;
    }
    printf("API listen socket ok \n");
        
ReAccept:

	printf("API Start Accept \n");
	new_server_socket = accept(server_socket, NULL,NULL);
    //new_server_socket = accept(server_socket, &client_addr, &sin_size);
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
    
    struct timeval timeout={1,0};
    
    //setsockopt(new_server_socket,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
    //setsockopt(new_server_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    buf =  (unsigned char*)malloc(sizeof(unsigned char)*BUFFER_SIZE);
   
    
    API_send(WELCOME);
    //API_send(WELCOME);  
    usleep(10000);
    while (1)
    {
        API_send(CMD_READY);
        
        API_recv(buf);
        cmd_parser(buf);
        printf("%s \n", buf);
        API_send(buf);
        memset(buf, 0, BUFFER_SIZE);
    }

	free(buf);
    close(new_server_socket);
    close(server_socket);

    return 0;
}

