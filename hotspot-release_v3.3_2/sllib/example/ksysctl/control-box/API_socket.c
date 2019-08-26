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
#include "Setting.h"
#include "API_socket.h"
#include "sharemem.h"

static char cmd[MAX_CMD_PART][MAX_LEN_PART] = {0};
static char buf[BUFFER_SIZE] = {0};
static char password[PASSWORD_SIZE] = {0};

static int API_send(int accept_fd, const void *buf)
{
    printf("API send sizeof(buf): %d \n", strlen(buf));
    int len = send(accept_fd, buf, strlen(buf)+1, 0);
    if (len <= 0)
    {
        perror("send");
        printf("Send len = %d \n",len);
        printf("API send errno : %d \n", errno);
        close(accept_fd);
        return -1;
    }
    return 0;
}

static int API_recv(int accept_fd, void *buf)
{
    printf("API receive sizeof(buf): %d \n", strlen(buf));
    int len = recv(accept_fd, buf, BUFFER_SIZE, 0);
    if (len <= 0)
    {
        perror("recv");
        printf("recv len = %d \n", len);
        printf("API recv errno : %d \n", errno);
        close(accept_fd);
        return -1;
    } 
    return 0;
}

static int cmd_split(char cmd[][MAX_LEN_PART], const char *str)
{
    char *pStrTmp = str;
    int cmd_count = 0;
    int m = 0, n = 0;
    printf("str: %s \n", str);
    while (*pStrTmp != '\0')
    {
        if ((*pStrTmp != ' ') && (*pStrTmp != '\r'))
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


static int alias_to_rx_index(const char *alias_rx)
{
    int rx_index = 0;
    printf("rx alias : %s", alias_rx);
    if (*alias_rx == '#')
    {
        alias_rx++;
        //printf("%s\n", alias_rx);
        rx_index = atoi(alias_rx); 
        if ((rx_index < 1) || (rx_index > 128))
        {
            printf("rx index: %d \n", rx_index);
            return -1;
        }
        else
        {
            return rx_index;
        }
    }
    else
    {
        for (rx_index=0; rx_index<128; rx_index++)
        {
            if ((0 == strcmp(share_mem->config_info.RX_Alias[rx_index], alias_rx)))
            {
                return rx_index;
            }
            else
            {
                return -1;
            }
        }
    }
}

static int alias_to_tx_position(const char *alias_tx)
{
    int tx_position = 0;

    if (*alias_tx == '#')
    {
        alias_tx++;
        printf("%s\n", alias_tx);
        tx_position = atoi(alias_tx); 
        if ((tx_position < 1) || (tx_position > 24))
        {
            return -1;
        }
        else
        {
            return tx_position;
        }
    }
    else
    {
        for (tx_position=0; tx_position<128; tx_position++)
        {
            if ((0 == strcmp(share_mem->config_info.RX_Alias[tx_position], alias_tx)))
            {
                return tx_position;
            }
            else
            {
                return -1;
            }
        }
    }
}

static int alias_to_group_index(const char *alias_group)
{
    int group_index = 0;

    if (*alias_group == '#')
    {
        alias_group++;
        //printf("%s\n", alias_rx);
        group_index = atoi(alias_group); 
        if ((group_index < 1) || (group_index > 20))
        {
            return -1;
        }
        else
        {
            return group_index;
        }
    }
    else
    {
        for (group_index=0; group_index<20; group_index++)
        {
            if ((0 == strcmp(share_mem->config_info.group[group_index].group_alias, alias_group)))
            {
                return group_index;
            }
            else
            {
                return -1;
            }
        }
    } 
}

static int alias_to_scene_index(const char *alias_scene)
{
    int scene_index = 0, ret;

    if (*alias_scene == '#')
    {
        alias_scene++;
        //printf("%s\n", alias_rx);
        scene_index = atoi(alias_scene); 
        if ((scene_index < 1) || (scene_index > 10))
        {
            return -1;
        }
        else
        {
            return scene_index;
        }
    }
    else
    {
        for (scene_index=0; scene_index<10; scene_index++)
        {
            if ((0 == strcmp(share_mem->config_info.scene[scene_index].scene_alias, alias_scene)))
            {
                return scene_index;
            }
            else
            {
                return -1;
            }
        }
    }
}

//
static int get_tx_matrix_from_rx(const int rx_index)
{
    if ((rx_index < 1) || (rx_index > 128))
    {
        printf("gat tx matrix from rx : %d \n", rx_index);
        printf("get tx matrix from rx function error \n");
        return -1;
    }

    int index = rx_index - 1;
    int video_source = share_mem->rx_info[index].video_source;

    sprintf(buf, "matrix get RX %s #%d \r\n", cmd[3], video_source);
    
    return 0;
}

static int get_rx_matrix_list_from_tx(const int tx_position)
{
    if ((tx_position < 1) || (tx_position) > 24)
    {
        printf("get rx matrix list from tx : %d \n", tx_position);
        printf("get rx matrix list from tx function error \n");
        return -1;
    }
    int video_source = tx_position;
    int i;
    char str_rx[20*128] = {0};
    char str_tmp[20] = {0};
    for (i=0; i<128; i++)
    {
        if (video_source == share_mem->rx_info[i].video_source)
        {
            sprintf(str_tmp, "#%d ", i+1);
            strcat(str_rx, str_tmp);
        }
    }
    sprintf(buf, "matrix get TX %s %s \r\n", cmd[3], str_rx);
    
    return 0;
}


/*
* display_group : 0-19
* tx : 0-24
*/
static int set_display_group_to_tx(const int display_group, const int tx_position)
{
    if ((display_group < 1) || (display_group > 10) || (tx_position < 1) || (tx_position > 24))
    {
        printf("set display geroup to tx eroro \n");
        return -1;
    }
    int group = display_group - 1;
    int video_source = tx_position;

    int count = 0;

    while (ON == share_mem->ucUpdateFlag)
    {
        usleep(10000);
        count++;
        if (count > 200)
        {
            printf("share memory update flag timeout \n");
            return -1;
        }
    }
    int i;
    for (i=0; i<128; i++)
    {
        if (1 == share_mem->config_info.group[group].group_member[i])
        {
            share_mem->rx_info[i].video_source = video_source;
        }
    }
    share_mem->ucUpdateFlag = ON;

    return 0;
}



static int get_single_TX_status(const int TX_position)
{
    int position = TX_position-1;
    char resolution[20] = {0};
    char audio_type[10] = {0};
    char audio_sample = 0;
    char aliasname[20] = {0};
    char online[10] = {0};
    char hdcp[20] = {0};
    char fw_version[20] = {0};
    char fw_status[10] = {0};
    printf("get single tx : %d \n", position);
    printf("share tx %d info hdmi input is %d \n", position, share_mem->tx_info[position].is_hdmi_input);
    if (share_mem->tx_info[position].is_hdmi_input)
    {
        sprintf(aliasname, "%s", share_mem->config_info.TX_Alias[position]);
        sprintf(online, "online");
        sprintf(resolution, "%d*%d", share_mem->tx_info[position].video_width, share_mem->tx_info[position].video_height);
        sprintf(audio_type, "PCM"); //FIXME
        audio_sample = share_mem->tx_info[position].audio_sample;
        sprintf(hdcp, "PCM"); //FIXME
        sprintf(fw_version, "%s", share_mem->tx_info[position].fw_version); // 
        sprintf(fw_status, "OK"); //FIXME
    }
    else
    {
        sprintf(aliasname, "%s", share_mem->config_info.TX_Alias[position]);
        sprintf(online, "offline");
        sprintf(resolution, " ");
        sprintf(audio_type, " ");
        audio_sample = 0;
        sprintf(hdcp, " ");
        sprintf(fw_version, " ");
        sprintf(fw_status, " ");
    }

    sprintf(buf,\
"{\r\n\
    \"status\"\r\n\
    [\r\n\
        {\r\n\
            \"id\":\"%d\"\r\n\
            \"aliasname\":\"%s\"\r\n\
            \"online\":\"%s\"\r\n\
            \"resolutioin\":\"%s\"\r\n\
            \"audio_type\":\"%s\"\r\n\
            \"audio_sample\":\"%d\"\r\n\
            \"hdcp\":\"%s\"\r\n\
            \"fw_version\":\"%s\"\r\n\
            \"fw_status\":\"%s\"\r\n\
        }\r\n\
    ]\r\n\
}\r\n",\
    TX_position, aliasname, online, resolution, audio_type, audio_sample, hdcp, fw_version, fw_status);

    return 0;
}

static int get_single_RX_status(const int RX_index)
{
    int index = RX_index - 1;

    char aliasname[20] = {0};
    char online[10] = {0};
    char fw_version[20] = {0};
    char fw_status[10] = {0};
    if (share_mem->rx_info[index].online_count)
    {
        sprintf(aliasname, "%s", share_mem->config_info.RX_Alias[index]);
        sprintf(online, "online");
        sprintf(fw_version, "%s", share_mem->rx_info[index].fw_version);
        sprintf(fw_status, "OK"); //FIXME
    }
    else
    {
        sprintf(aliasname, "%s", share_mem->config_info.RX_Alias[index]);
        sprintf(online, "offline");
        sprintf(fw_version, " ");
        sprintf(fw_status, " ");  
    }
    
    //printf("get single rx status : %d \n", RX_index);
    sprintf(buf,\
"{\r\n\
    \"status\"\r\n\
    [\r\n\
        {\r\n\
            \"id\":\"%d\"\r\n\
            \"aliasname\":\"%s\"\r\n\
            \"online\":\"%s\"\r\n\
            \"fw_version\":\"%s\"\r\n\
            \"fw_status\":\"%s\"\r\n\
        }\r\n\
    ]\r\n\
}\r\n",\
    RX_index, aliasname, online, fw_version, fw_status);
}

static int get_HUB_status(const int HUB_index)
{
    //TODO
}

static int get_single_EXT_status(const int EXT_ID)
{
    //TODO

}

static int set_scene_enable(const int scene_index)
{
    int count = 0;

    if ((scene_index < 0) || (scene_index > 10))
    {
        printf("set scene enable error \n");
        printf("set scene %d enable \n", scene_index);
        return -1;
    }
    while (ON == share_mem->ucUpdateFlag)
    {
        usleep(10000);
        count++;
        if (count > 200)
        {
            printf("share memory update flag timeout \n");
            return -1;
        }
    }
    
    int i;
    int index = scene_index - 1;
    for (i=0; i<10; i++)
    {
        if (1 == share_mem->config_info.scene[index].scene_member[i])
        {
            set_display_group_to_tx(i, share_mem->config_info.scene[index].scene_source);
        }
    }
    share_mem->ucUpdateFlag = ON;
    return -1; 
}


static int set_matrix_command(int tx_position, int cmd_count)
{
    if ((tx_position < 1) || (tx_position > 24) || (cmd_count > 128))
    {
        printf("tx position : %d , cmd_count  : %d \n", tx_position, cmd_count);
        printf("set matrix command function error \n");
        return -1;
    }
    int video_source = tx_position;
    int index;
    int i;

    while (ON == share_mem->ucUpdateFlag)
    {
        usleep(10000);
    }

    for (i=3; i=cmd_count; i++) 
    {
        index = alias_to_group_index(cmd[i]);
        if (index < 0)
        {
            printf("alias to group index error \n");strcpy(buf, ALERT_PARA_ERROR); 
            return -1;  
        }
        index--;
        share_mem->rx_info[index].video_source = video_source;
    }
    
    share_mem->ucUpdateFlag = ON;

    return 0;
}

static int set_OSD_display(const char OSD_state)
{
    int i;
    printf("set_OSD_display \n");
    while (share_mem->ucUpdateFlag == ON)
    {
        usleep(10000);
    }
    if (OFF == OSD_state)
    {
        for (i=0; i<128; i++)
        {
            share_mem->rx_info[i].osd_status = OFF;
        }
    }
    if (ON == OSD_state)
    {
        for (i=0; i<128; i++)
        {
            share_mem->rx_info[i].osd_status = ON;
        }
    }
    share_mem->ucUpdateFlag = ON;

    return 0;
}

static int set_display_group_power(const int power_state, const int cmd_count)
{
    if (cmd_count > 128)
    {
        printf("set_display_group_power cmd_count  : %d \n", cmd_count);
        printf("set_display_group_power function error \n");
        return -1;
    }
    int tv_status = power_state;
    int index;
    int i, j;

    while (ON == share_mem->ucUpdateFlag)
    {
        usleep(10000);
    }

    for (i=5; i<cmd_count; i++) 
    {
        index = alias_to_group_index(cmd[i]);
        if (index < 0)
        {
            printf("alias to group index error \n");
            strcpy(buf, ALERT_PARA_ERROR); 
            return -1;  
        }
        index--;
        for (j=0; j<128; j++)
        {
            if ((1 == (share_mem->config_info.group[index].group_member[j])))
            {
                share_mem->rx_info[j].tv_status = tv_status;
            }
        }
    }
    
    share_mem->ucUpdateFlag = ON;

    return 0;    

}

static int set_sink_power(const int power_state, const int cmd_count)
{
    if (cmd_count > 128)
    {
        printf("set_sink_power cmd_count : %d \n", cmd_count);
        printf("set matrix command function error \n");
        return -1;
    }
    int tv_status = power_state;
    printf("tv_satus : %d \n", tv_status);
    int index;
    int i;

    while (ON == share_mem->ucUpdateFlag)
    {
        usleep(10000);
    }

    for (i=5; i<cmd_count; i++) 
    {
        index = alias_to_rx_index(cmd[i]);
        if (index < 0)
        {
            printf("alias to rx index error \n");
            strcpy(buf, ALERT_PARA_ERROR); 
            return -1;  
        }
        index--;
        share_mem->rx_info[index].tv_status = tv_status;
        printf("share memory tv status : %d \n", share_mem->rx_info[index].tv_status);
    }
    
    share_mem->ucUpdateFlag = ON;
    printf("set sink power \n");

    return 0;
}

static int all_reboot(void)
{
    int i;
    for (i=0; i<128; i++)
    {
        
    }
}

static int set_notify_state(const int state)
{
    //TODO
}

static void cmd_parser(char *str)
{
    int cmd_count = 0, i, ret;
    cmd_count = cmd_split(cmd, str);

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
        else if ((0 == strcmp(cmd[1], GET))) //matrix get 
        {

            if (cmd_count > 4)
            {
                strcpy(buf, ALERT_PARA_MORE);
                return; 
            }

            //get RX 
            //command : matrix get RX <RX> 
            //response : matrix get RX <RX> <TX>
            else if ((0 == strcmp(cmd[2], RX))) //DONE
            {
                int rx_index = alias_to_rx_index(cmd[3]);
                if (rx_index < 0)
                {
                    //error 
                    strcpy(buf, ALERT_PARA_ERROR); 
                    return ;
                }
                else
                {
                    get_tx_matrix_from_rx(rx_index);
                }
            }
            
            //get TX
            //command : matrix get TX <TX>
            //response : matrix get TX <TX> <RX1> <RX2> ... <RXn>
            else if ((0 == strcmp(cmd[2], TX))) //DONE
            {
                int position = alias_to_tx_position(cmd[3]);
                if (position < 0)
                {
                    strcpy(buf, ALERT_PARA_ERROR); 
                    return ; 
                }
                get_rx_matrix_list_from_tx(position);
            }
            else
            {
                strcpy(buf, SAMPLE_MATRIX);
                return;
            }
        }
        //matrix set
        else if ((0 == strcmp(cmd[1], SET))) //DONE
        {
            printf("SET \n");
            //command : matrix set <TX> <RX1> <RX2> .... <RXn>
            //response : matrix set <TX> <RX1> <RX2> .... <RXn>
            
            if (cmd_count < 4)
            {
                strcpy(buf, SAMPLE_MATRIX);
                return ;
            }
            else 
            {
                int position = alias_to_tx_position(cmd[2]);
                if (position < 0)
                {
                    strcpy(buf, SAMPLE_MATRIX);
                    return ; 
                }
                ret = set_matrix_command(position, cmd_count);
                if (ret < 0)
                {
                    strcpy(buf, ALERT_PARA_ERROR); 
                    return ;  
                }
            }
        }
        else
        {
           strcpy(buf, SAMPLE_MATRIX);
           return ;
        }
    }
    //scene
    else if ((0 == strcmp(cmd[0], SCENE)))
    {
        printf("SCENE \n");
        if ((cmd_count < 2) || (cmd_count > 3))
        {
            strcpy(buf, SAMPLE_SCENE);
            return;
        }
        //scene
        //command : scene set <S>
        //response : scene set <S>
        else if ((0 == strcmp(cmd[1], SET))) //DONE
        {
            int scene_index = alias_to_scene_index(cmd[2]);
            if (scene_index < 0)
            {
                strcpy(buf, SAMPLE_SCENE);
                return ;
            }
            else
            {
                set_scene_enable(scene_index);
            }
        }
        else
        {
           strcpy(buf, SAMPLE_SCENE);
        }
    }
    //displaygroup
    else if ((0 == strcmp(cmd[0], DISPLAY))) //DONE
    {
        printf("DISPLAY \n");
        //displaygroup
        //command : displaygroup set <DG> <TX>
        //response : displaygroup set <DG> <TX>
        if (cmd_count > 4)
        {
            strcpy(buf, SAMPLE_GROUP);
            return; 
        }
        else if ((0 == strcmp(cmd[1], SET))) //displaygroup set 
        {
            int group_index = alias_to_group_index(cmd[2]);
            int tx_position = alias_to_tx_position(cmd[3]);
            if ((group_index < 0) || (tx_position < 0))
            {
                strcpy(buf, SAMPLE_GROUP); 
                return ; 
            }
            set_display_group_to_tx(group_index, tx_position);
        }
        else
        {
           strcpy(buf, SAMPLE_GROUP); 
        } 
    }
    //config 
    else if ((0 == strcmp(cmd[0], CONFIG))) 
    {
        printf("CONFIG \n");
        if (cmd_count < 2)
        {
            strcpy(buf, SAMPLE_CONFIG);
            return;
        }
        //config 
        //command : config 
        else if ((0 == strcmp(cmd[1], GET))) //config get
        {
            printf("GET \n"); 
            if (cmd_count < 5)
            {
                strcpy(buf, SAMPLE_CONFIG);
                return;
            } 
            //DONE
            //config get TX status <TX>
            else if ((0 == strcmp(cmd[2], TX))) 
            {
                printf("TX \n");
                if ((0 == strcmp(cmd[3], STATUS)))
                {
                    int position = alias_to_tx_position(cmd[4]);
                    if (position < 0)
                    {
                        strcpy(buf, SAMPLE_CONFIG);
                        return; 
                    }
                    else
                    {
                        get_single_TX_status(position);
                    }
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return; 
                }
            }
            //config get RX status <RX>
            else if ((0 == strcmp(cmd[2], RX)))
            {
                printf("RX \n");
                if ((0 == strcmp(cmd[3], STATUS)))
                {
                    int index = alias_to_rx_index(cmd[4]);
                    if (index < 0)
                    {
                        strcpy(buf, ALERT_PARA_ERROR);
                        return; 
                    }
                    else
                    {
                        get_single_RX_status(index);
                    }
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return; 
                }
            }
            //config get HUB status
            else if ((0 == strcmp(cmd[2], HUB)))
            {
                if ((0 == strcmp(cmd[3], STATUS)))
                {
                    get_HUB_status(cmd[4]);
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return; 
                }
            }
            //config get EXT status <EXT>
            else if ((0 == strcmp(cmd[2], EXT)))
            {
                if ((0 == strcmp(cmd[3], STATUS)))
                {
                    get_single_EXT_status(cmd[4]);
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return; 
                }
            }
            else 
            {
                strcpy(buf, SAMPLE_CONFIG);
                return;
            }
        }
        //command : config set
        else if ((0 == strcmp(cmd[1], SET))) //config set
        {
            printf("SET \n");
           
            //config set OSD
            //command : config set OSD <on|off>
            //response : config set OSD <on|off>
            if ((0 == strcmp(cmd[2], OSD)))
            {
                printf("OSD\n");
                if ((0 == strcmp(cmd[3], OFF_STR)))
                {
                    set_OSD_display(OFF);
                }
                else if ((0 == strcmp(cmd[3], ON_STR)))
                {
                    set_OSD_display(ON);
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return; 
                }
            }

            //config set reboot
            //command : config set reboot
            //response : config set reboot
            else if ((0 == strcmp(cmd[2], REBOOT)))
            {
                printf("REBOOT\n");
                all_reboot();
            }

            //config set notify
            //command : config set notify <on|off>
            //response : config set notify <on|off>
            else if ((0 == strcmp(cmd[2], NOTIFY)))
            {
                printf("NOTIFY\n");
                if ((0 == strcmp(cmd[3], OFF_STR)))
                {
                    set_notify_state(OFF);

                }
                else if ((0 == strcmp(cmd[3], ON_STR)))
                {
                    set_notify_state(ON);
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return;  
                }
                
            }

            //config set device
            //command : config set device sinkpower <on|off> <RX1> <RX2> ... <RXn>
            //response : config set device sinkpower <on|off> <RX1> <RX2> ... <RXn> 
            else if ((0 == strcmp(cmd[2], DEVICE)))
            {
                printf("DEVICE\n");
                if ((0 == strcmp(cmd[3], SINK_POWER)))
                {
                    if ((0 == strcmp(cmd[4], OFF_STR)))
                    {
                        set_sink_power(OFF, cmd_count);
                    }
                    else if ((0 == strcmp(cmd[4], ON_STR)))
                    {
                        set_sink_power(ON, cmd_count);
                    }
                    else
                    {
                        strcpy(buf, SAMPLE_CONFIG);
                        return;  
                    }
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return; 
                }
            }

            //config set displaygroup
            //command : config set displaygroup sinkpower <on|off> <DG1> <DG2> .... <DGn>
            //response : config set displaygroup sinkpower <on|off> <DG1> <DG2> .... <DGn> 
            else if ((0 == strcmp(cmd[2], DISPLAY_GROUD)))
            {
                printf("DISPLAY_GROUD\n");
                if ((0 == strcmp(cmd[3], SINK_POWER)))
                {
                    if ((0 == strcmp(cmd[4], ON_STR)))
                    {
                        set_display_group_power(ON, cmd_count);
                    }
                    else if ((0 == strcmp(cmd[4], OFF_STR)))
                    {
                        set_display_group_power(OFF, cmd_count);
                    }
                    else
                    {
                        strcpy(buf, SAMPLE_CONFIG);
                        return; 
                    }
                }
                else
                {
                    strcpy(buf, SAMPLE_CONFIG);
                    return; 
                }
                
            }
            else
            {
                strcpy(buf, SAMPLE_CONFIG);
                return; 
            }
        }
        else
        {
           strcpy(buf, SAMPLE_CONFIG); 
        } 
    }
    else 
    {
        strcpy(buf, UNKNOW);
    }

    return;
}

void API_accept(int accept_fd)
{
    //printf();
    int socket_id = accept_fd;
    
    memset(password, 0, PASSWORD_SIZE);
    strcat(password, share_mem->password);
    strcat(password, "\r\n\0");
Repassword:
    memset(buf, 0, BUFFER_SIZE);
    if (API_send(socket_id, PASSWORD) < 0)
    {
        return ;
    }
    if (API_recv(socket_id, buf) < 0)
    {
        return ;
    }
    
    if (strcmp(password, buf) != 0)
    {
        memset(buf, 0, BUFFER_SIZE); 
        strcpy(buf, PASS_ERROR);
        if (API_send(socket_id, buf) < 0)
        {
            return ;
        }
        
        goto Repassword;
    }
    if (API_send(socket_id, WELCOME) < 0)
    {
        return ;
    }
    
    usleep(10000);
    while (1)
    {
        if (API_send(socket_id, CMD_READY) < 0)
        {
            return ;
        }
        
        if (API_recv(socket_id, buf) < 0)
        {
            return ;
        }
        cmd_parser(buf);
        printf("%s \n", buf);
        if (API_send(socket_id, buf) < 0)
        {
            return ;
        }
        memset(buf, 0, BUFFER_SIZE);
    }

    close(socket_id);
    return ;
}

void *API_server(void)
{
	//sockfd
	int server_socket, new_server_socket;

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
    fd_set  select_set;
    pthread_t thread_accept = 0;
    while (1)
    {
        FD_ZERO(&select_set);
		FD_SET(server_socket,&select_set);
		select(server_socket+1,&select_set,NULL,NULL,NULL);
        if(FD_ISSET(server_socket,&select_set))
		{
ReAccept:
            printf("API Start Accept \n");
            new_server_socket = accept(server_socket, NULL,NULL);
            //new_server_socket = accept(server_socket, &client_addr, &sin_size);
            if (new_server_socket < 0)
            {
                printf("Server API Accept Failed!\n");
                perror("accept");
                printf("accept errno : %d \n", errno);
                close(server_socket);
                sleep(1);
                goto ReAccept;
            }
            else
            {
                printf("Server API Aceept OK , new socket = %d\n", new_server_socket);
                pthread_create(&thread_accept, NULL, API_accept, new_server_socket);	
				pthread_detach(thread_accept);
            }
        }
    }
	
    close(server_socket);

    return 0;
}

