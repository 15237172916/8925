#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <sys/mman.h>
#include <sl_types.h>
#include <sl_error.h>
#include <sl_api_filedev.h>
#include <sl_api_ksysctl.h>
#include <sl_api_memdev.h>
#include <sl_api_viu.h>
#include <sl_param_vcodec.h>
#include <sl_api_vcodec.h>
#include <vpp/sl_param_pv.h>
#include <vpp/sl_api_pv.h>
#include <sl_api_vpre.h>
#include <sl_param_vpre.h>
#include <pthread.h>
#include <sl_debug.h>
#include <sl_mmutil.h>
#include <sl_file.h>
#include <sl_rtsp.h>
#include <linux/soundcard.h>
#include "sl_watchdog.h"
#include "list_handler.h"
#include "cfginfo.h"

#include "ir.h"

#define IR_DEBUG

#define MYPORT 8000
#define LENGTH_OF_LISTEN_QUEUE  20
#define HELLO_WORLD_SERVER_PORT 8080

#define IR_DATA_LEN IR_DATA_BUF_SIZE
#define IR_DATA_LEN_OFFSET (IR_DATA_BUF_SIZE - 16)

static int ir_fd = -1;
static int cm0_fd = -1;

extern SL_U32 interface;

int ir_recv_buf[IR_DATA_LEN];
int ir_send_buf[IR_DATA_LEN];

int ir_info_buf_addr;
int ir_recv_info_buf_addr;
int ir_send_info_buf_addr;
int ir_recv_data_buf_addr;
int ir_send_data_buf_addr;

struct ir_info_s *ir_recv_info, *ir_send_info;

int ir_init_info(struct ir_info_s *info)
{
    if (info == NULL)
    {
        printf("ir_init_info failed, info NULL\n");
        return -1;
    }

    if (info == ir_recv_info)
        info->buf_ck = ir_recv_data_buf_addr;
    else if (info == ir_send_info)
        info->buf_ck = ir_send_data_buf_addr;
    else
    {
        printf("ir_init_info failed, info 0x%x\n", (int)info);
        return -1;
    }

    memset((char *)info->buf_ck, 0, IR_DATA_BUF_SIZE*4);
    info->state = STATE_IDLE;
    info->data_cnt = 0;
    info->wave_cnt = 0;
    info->data_max = 0;

    return 0;
}

void ir_init_buf(int start_addr)
{
    ir_info_buf_addr = start_addr;
    ir_recv_info_buf_addr = ir_info_buf_addr;
    ir_send_info_buf_addr = ir_recv_info_buf_addr + IR_INFO_BUF_SIZE*4;
    ir_recv_data_buf_addr = ir_send_info_buf_addr + IR_INFO_BUF_SIZE*4;
    ir_send_data_buf_addr = ir_recv_data_buf_addr + IR_DATA_BUF_SIZE*4;
}

int ir_init(void)
{
    if (ir_fd >= 0)
        close(ir_fd);

    ir_fd = open("/dev/silan-dsp-ir", O_RDWR);
    if(ir_fd < 0)
    {
        printf("fail to open /dev/silan-dsp-ir\n");
        return -1;
    }

    unsigned int addr = (unsigned long)mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, ir_fd, 0);//map 4k bytes

    memset((unsigned char *)addr, 0x00, 0x1000);

    ir_init_buf(addr);

    ir_recv_info = (struct ir_info_s *)ir_recv_info_buf_addr;
    ir_init_info(ir_recv_info);

    ir_send_info = (struct ir_info_s *)ir_send_info_buf_addr;
    ir_init_info(ir_send_info);

    if (cm0_fd >= 0)
        close(cm0_fd);

    cm0_fd = open("/dev/silan-cm0", O_RDWR);
    if (cm0_fd < 0)
    {
        printf("fail to open /dev/silan-cm0\n");
        return -1;
    }
	
    return 0;
}



//get ir from net and set m0 to transfer
SL_POINTER get_ir(SL_POINTER Args)
{
    struct ifreq if0;
    unsigned char *buffer;
    int need_recv, len, pos;
    int server_socket;
    int new_server_socket;
    socklen_t length;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int *tx_buf, tx_len;
    struct ir_info_s *rx_info, *tx_info;
    int sin_size;
	
    if (ir_init())
        reboot1();

    printf ("%s started.\n", __func__);
    printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);

    // create a stream socket
    server_socket = socket(PF_INET, SOCK_DGRAM, 0); //create udp socket
    if (server_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }

    if (interface == INTERFACE_WLAN0)
    {
        strncpy(if0.ifr_name, "ra0", IFNAMSIZ);
        if (ioctl(server_socket, SIOCGIFHWADDR, &if0)<0)
        {
            printf("get_ir ioctl SIOCGIFHWADDR error\n");
            exit(1);
        }
    }
    int on=1;
	int ser_ret=setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	if(ser_ret<0)
	{
		perror("setsockopt failure!");
		exit(0);
	}
	else
	{
		printf("set socket option sucess!\n");
	}
    //bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);
        exit(1);
    }

    need_recv = IR_DATA_LEN*4;   //256*4
    buffer = (unsigned char*)ir_recv_buf;
    memset(buffer, 0x00, need_recv);
    //bzero(buffer,need_recv);
    sin_size=sizeof(struct sockaddr_in);
    while (1)
    {
        pos = 0;
        while (pos < need_recv)
        {
            //len = recv(new_server_socket, buffer + pos, need_recv - pos, 0);//block receive   tcp
            len=recvfrom(server_socket,buffer+pos,need_recv-pos,0,(struct sockaddr*)&client_addr,&sin_size);  //server recv client data udp
            if (len < 0)
            {
                printf("Server Recieve Data Failed!\n");
                break;
            }

            pos += len;
        }

        printf("ir rev len %d\n", pos);
        if (need_recv != pos)
            printf("need_recv %d len %d\n",need_recv, pos);

#ifdef IR_DEBUG
        int i;

        printf("ir recv:\n");
        for (i = 0;i < IR_DATA_LEN; i++)
        {
            printf("%d ", ir_recv_buf[i]);
        }
        printf("\n");
#endif

        tx_info = ir_send_info;
        rx_info = ir_recv_info;

        tx_buf = ir_recv_buf;
        tx_len = *(tx_buf + IR_DATA_LEN_OFFSET);
        ir_init_info(rx_info);
        tx_info->data_max = tx_len;
        tx_info->data_cnt = 0;
        tx_info->wave_cnt = 0;
        memcpy((char *)tx_info->buf_ck, (char *)tx_buf, IR_DATA_LEN*4);
        tx_info->state = STATE_SEND_DONE;
		
        usleep(20000);
    }
	//printf("test!\n");
   // close(new_server_socket);
    close(server_socket);

    return 0;
}


//get ir from m0 and set to net
SL_POINTER send_ir(SL_POINTER Args)
{
    struct ifreq if0;
    int sock_cli, len, *pbuf;
    struct sockaddr_in servaddr;
    struct ir_info_s *rx_info, *tx_info;

    printf ("%s started.\n", __func__);
    printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );

    if (ir_init())
        reboot1();

try_again:
    sock_cli = socket(PF_INET,SOCK_DGRAM, 0); // create Udp socket

    if (interface == INTERFACE_WLAN0)
    {
        strncpy(if0.ifr_name,"ra0",IFNAMSIZ);
        if (ioctl(sock_cli,SIOCGIFHWADDR,&if0) < 0)
        {
            printf("send_ir ioctl SIOCGIFHWADDR error\n");
            return NULL;
        }
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);
    if(interface == INTERFACE_WLAN0)
        servaddr.sin_addr.s_addr = inet_addr("10.10.1.1");
    else
        servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        close(sock_cli);
        sleep(2);
        goto try_again;
    }

    printf("\n*** connect ok**\n");

    tx_info = ir_send_info;
    rx_info = ir_recv_info;
    pbuf = rx_info->buf_ck;
    while (1)
    {
        if (rx_info->state != STATE_RECV_DONE)
        {
            usleep(10000);
            continue;
        }

        memset((char *)ir_send_buf, 0, IR_DATA_LEN*4);
        memcpy((char *)ir_send_buf, (char *)rx_info->buf_ck, 4*rx_info->data_max);
        *(ir_send_buf + FREQ_OFFSET) = *(pbuf + FREQ_OFFSET);
        *(ir_send_buf + IR_DATA_LEN_OFFSET) = rx_info->data_max;
        ir_init_info(rx_info);

        printf("ir send start\n");
        len=sendto(sock_cli,ir_send_buf,IR_DATA_LEN*4,0,(struct sockaddr*)&servaddr,sizeof(servaddr));  //udp send data
        if (len <= 0)
        {
            perror("ERRPR");
            printf("re-connect!\n");
            close(sock_cli);
            sleep(2);
            goto try_again;
        }
        printf("ir send over\n");
        if (len != IR_DATA_LEN*4)
        {
           printf("actual send len:%d\n", len);
	   }
        usleep(10000);
    }
	
    close(sock_cli);

    return 0;
}


