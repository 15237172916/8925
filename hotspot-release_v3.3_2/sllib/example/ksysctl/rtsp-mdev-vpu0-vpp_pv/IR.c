#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/socket.h>                         // 包含套接字函数库
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>                         // 包含AF_INET相关结构
#include <math.h>
#include "../version.h"
#include "IR.h"
#include "sharemem.h"
#include "digit_led.h"


extern char multicast[20];
extern char web_flag;

static unsigned char *dsp_ir_start_addr_va;

static char shumaduan[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};  //0~f

static int init_dsp_ir(void)
{
    int audio_fd = -1;
    audio_fd=open("/dev/silan-dsp-ir", O_RDWR);
    if(audio_fd<0){
        printf("fail to open /dev/silan-dsp-ir\n");
        return -1;
    }

    unsigned int tmp =  (unsigned long)mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, audio_fd, 0); //map 8k

    dsp_ir_start_addr_va = (unsigned char *)tmp;

    printf("\n***dsp_ir_start_addr_va:%x\n",dsp_ir_start_addr_va);

    memset((unsigned char *)dsp_ir_start_addr_va,0x00,0x1000);

    return 0;
}

static int H_anjian(void)
{
#if 1
    char rx_ip[20];
    char str[20];
    int flag1,flag2;
    int n,tmp;
    
    strcpy(rx_ip,share_mem->sm_eth_setting.strEthIp);
    sscanf(rx_ip,"192.168.1.%d",&n);
   // printf("ip is : %d\n",n);

    flag1 = n/10;
    flag2 = n%10;

    flag1++;
    if((flag2==0)&&(flag1>12))
    {
        flag1=0;
        flag2=1;
    }
    if((flag2==9)&&(flag1>11))
    {
        flag2=0;
    }
    if((flag2!=0)&&(flag2!=9))
    {
        if(flag1>12)
          flag1=0;
    }
    printf("flag1:%d\n",flag1);
    digit_led_writebyte(0x48,0x11);
    digit_led_writebyte(0x68,shumaduan[flag1]);

    tmp=(flag1*10)+flag2;
    printf("new ip is :%d\n",tmp);
    sprintf(str,"192.168.1.%d",tmp);
    strcpy(share_mem->sm_eth_setting.strEthIp,str);

   sprintf(str,"239.255.42.%d",tmp);
   strcpy(share_mem->sm_eth_setting.strEthMulticast,str);
                
    return tmp;
#endif
}

static int U_anjian(void)
{
    char rx_ip[20];
    char str[20];
    int flag1,flag2;
    int n,tmp;
    
    strcpy(rx_ip,share_mem->sm_eth_setting.strEthIp);
    sscanf(rx_ip,"192.168.1.%d",&n);
   // printf("ip is : %d\n",n);

    flag1 = n/10;
    flag2 = n%10;

    flag2++;
        if((flag1==0)&&(flag2>9))
        {
             flag2=1;
        
        }
       if((flag1==12)&&(flag2 > 8))
       {
             flag2 = 0;
       }
       if((flag1!=0)&&(flag1!=12))
       {
          if(flag2>9)
           flag2=0;
       }
    printf("flag2:%d\n",flag2);
    digit_led_writebyte(0x48,0x11);
    digit_led_writebyte(0x6A,shumaduan[flag2]);

    tmp=(flag1*10)+flag2;
    printf("new ip is :%d\n",tmp);
    sprintf(str,"192.168.1.%d",tmp);
    strcpy(share_mem->sm_eth_setting.strEthIp,str);

    sprintf(str,"239.255.42.%d",tmp);
    strcpy(share_mem->sm_eth_setting.strEthMulticast,str);
                
    return tmp;

}

static int IR_bijiao(int *p)
{
    int n,i=34,j=35,k=7;
    int m,x=2,y=3,z=7;
    int flag1,flag2;
    int num=0;
    int addr=0;
    int tmp=0;

   /* 
    for(m=0;m<8;m++)
    {
            if((40<p[x]<43)&&(p[y]==44))
                printf("0 ");
            if((40<p[x]<43)&&(p[y]==129))
            {
                printf("1 ");
                addr+=(pow(2,z));
            }
            z--;
            x+=2;
            y+=2;
    }
    printf("addr:%d\n",addr);
    */
    for(n=0;n<8;n++)
    {
        if((40<p[i]<43)&&(p[j]==44))
            printf("0 ");
        if((40<p[i]<43)&&(p[j]==129))
        {
            printf("1 ");
            num+=(pow(2,k));
        }
        k--;
        i+=2;
        j+=2;
    }
    printf("num :%d\n",num);

    switch(num)
    {
        case 200:
            printf("anjian H anxia ! \n");
           tmp=H_anjian();
        
            break;
        case 64:
            printf("anjian U anxia ! \n");
           tmp=U_anjian();
            break;
        case 72:
            printf("dianyuan anixa !\n");
            break;
        case 104:
            printf("dianyuan +1 anxia !\n");
            break;
        case 88:
            printf("dianyuan +2 anxia !\n");
            break;
        case 128:
            printf("A1 anxia !\n");
            tmp=1;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 120:
            printf("A2 anxia !\n");
            tmp=2;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 192:
            printf("A+ anxia !\n");
            tmp=3;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 32:
            printf("A3 anxia !\n");
            tmp=4;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 160:
            printf("A4 anxia !\n");
            tmp=5;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 96:
            printf("A- anxia !\n");
            tmp=6;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 224:
            printf("B1 anxia !\n");
            tmp=7;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 16:
            printf("B2 anxia !\n");
            tmp=8;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 144:
            printf("B+ anxxia !\n");
            tmp=9;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 80:
            printf("B3 anxia !\n");
            tmp=10;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 208:
            printf("B4 anxia !\n");
            tmp=11;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 248:
            printf("B- anxia !\n");
           tmp=12;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 48:
            printf("X anxia !\n");
            tmp=13;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 176:
            printf("Y anxia !\n");
            tmp=14;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;
        case 112:
            printf("Z anxia !\n");
            tmp=15;
            flag1=tmp/10;
            flag2=tmp%10;
            digit_led_writebyte(0x48,0x11);
            digit_led_writebyte(0x68,shumaduan[flag1]);
            digit_led_writebyte(0x6A,shumaduan[flag2]);
            break;

        default: break;
    }
    return tmp;

}

#if 1
SL_POINTER  send_ir(SL_POINTER Args)
{
    ///sockfd
    //struct ifreq if0;
    int sock_cli;
    static int offset = 0;
    static int value = 0;
    static int cnt = 0;
    int time=0;
    static int new_ip,old_ip;
    
    char rx_ip[20];
    char str[20];
    int flag1,flag2;
    int ip,three,ret;
    int new_multicast,old_multicast;
    
    strcpy(rx_ip,share_mem->sm_eth_setting.strEthIp);
    sscanf(rx_ip,"192.168.1.%d",&ip);
    old_multicast=ip;

    flag1 = ip/10;
    flag2 = ip%10;
    old_ip=flag1*10+flag2;
    new_ip=old_ip;
    printf("old_ip :%d\n",old_ip);
    printf("old_multicast:%d\n",old_multicast);

    printf ("%s started.\n", __func__);
    printf ("%s started. pid %ld ....\n", __func__, syscall(SYS_gettid) );
    int needSend=72*3*2;
    unsigned char *dst=(unsigned char*)malloc(sizeof(Node));
    unsigned short *dst1=(unsigned short*)malloc(72*3*2);
    int i;
    int j;

    init_dsp_ir();
#if 1
    socklen_t servlen_addr_length;
    struct sockaddr_in server_addr;
    servlen_addr_length = sizeof(server_addr);
                
ReSocket:
    web_flag = 0;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);
    //server_addr.sin_addr.s_addr=htonl(INADDR_BROADCAST); //UDP broadcast address 
    //server_addr.sin_addr.s_addr=inet_addr(MCAST_ADDR); //UDP multicast address
    server_addr.sin_addr.s_addr=inet_addr(multicast); //multicast 

    sock_cli = socket(AF_INET,SOCK_DGRAM, 0); //UDP
    if (sock_cli < 0)
    {
        perror("socket");
        sleep(1);
        goto ReSocket;
    }
#if 1
    int reuse = 1;
    ret = setsockopt(sock_cli, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
    if(ret < 0)
    {
        printf("set multicast reuse error \n");
        perror("setsockopt");
    }
#endif

#if 0
    char outfilename[128] = "./bin.txt";
    static FILE * outfile  = NULL;
    if(!outfile)
    {
        if ((outfile = fopen(outfilename, "wt")) == NULL)
        {
            printf("can't open %s",outfilename);
            return;
        }
    }
#endif
#endif
    while(1)
    {
        int m,x=2,y=3,z=7;
        int addr=0;
        
        char tt="hello";
        char NEW[20];
        time++; 
        usleep(100000); //0.1s
            
        if((time>30)&&(new_ip!=old_ip)&&(new_ip!=0))   //3s 
        {
            printf("<1> old_ip :%d ,new_ip :%d\n",old_ip,new_ip);
            old_ip=new_ip;
            printf("<><>< |||| >><<<<<<<>>>>>>><< |||| ><><> \n");
            printf("<2> old_ip :%d ,new_ip :%d\n",old_ip,new_ip);

            sprintf(str,"192.168.1.%d",new_ip);
            strcpy(share_mem->sm_eth_setting.strEthIp,str);

            sprintf(str,"239.255.42.%d",new_ip);
            strcpy(share_mem->sm_eth_setting.strEthMulticast,str);
            
            flag1=new_ip/10;
            flag2=new_ip%10;
            printf("flag1:%d,flag2:%d\n",flag1,flag2);

            for(three=0;three<3;three++)
            {
                usleep(600000);
                digit_led_writebyte(0x6A,0xff);
                digit_led_writebyte(0x68,0xff);

                usleep(300000);
                digit_led_writebyte(0x6A,shumaduan[flag2]);
                digit_led_writebyte(0x68,shumaduan[flag1]);
            }

            AppWriteCfgInfotoFile();
            init_eth();   
            share_mem->ucUpdateFlag = 1;
            time=0;
            printf("new_ip write is ok!\n");
        }

        if((*(dsp_ir_start_addr_va + offset)) != 0)
        {
            usleep(100000);//FIXME let m0 capture entire data 0.1s
            memcpy(dst, (unsigned char *)(dsp_ir_start_addr_va + offset),IR_DATA_LENGTH);
            int count1;
            //int addr=0;
            int arr[216];
            char bin;
            //int m,x=2,y=3,z=7;
            for (count1=0; count1<IR_DATA_LENGTH; count1++)
            {
                //int tmp;
                //int tmp1 = dsp_ir_start_addr_va[count1];
#if 0
                for (tmp=8; tmp>0; tmp--)
                {
                    bin = tmp1 & 0x80;
                    if (bin == 0x80)
                    {
                        fputc('1', outfile);
                        fputc('\n', outfile);
                        //fprintf(outfile, "%d\n", tmp);
                    }
                    else
                    {
                        fputc('0', outfile);
                        fputc('\n', outfile);
                        //fprintf(outfile, "%d\n", tmp);
                    }
                    tmp1 = tmp1 << 1;
                }
#endif
#if 0
                if (count1%20 == 0)
                {
                    printf("\n");
                }
                printf("%x ", dst[count1]);
#endif
            }
            printf("fclose**********************\n\n");
            //fclose(outfile);
            memset((unsigned char *)(dsp_ir_start_addr_va + offset),0x00,IR_DATA_LENGTH); //FIXME
            memset((unsigned char *)dst1,0x00,72*3*2); //FIXME
#ifdef IR_DEBUG
            printf("dst[0]:%x\n",dst[0]);
            printf("dst[1]:%x\n",dst[1]);
            printf("dst[2]:%x\n",dst[2]);
#endif
            dst1[214] = dst[2] * 50; //FIXME 
            //dst1[70] = dst[2] * 100 / 2; //FIXME 
#if 1
            j=0;
            i=0;
            while(i<=IR_DATA_LENGTH) //IR_DATA_LENGTH : 2040
            {
                if(*(dst + i) == 0xff)
                {
                    if(value > 0)
                    {
#ifdef IR_DEBUG
                        printf("-%d ",value);
#endif
                        dst1[j] = value* 2 /dst[2];
                        j++;
                        value = 0;
                    }
                    if(i%2)
                    {
#ifdef IR_DEBUG
                        printf("*%d ",*(unsigned short *)(dst + i + 1));
#endif
                        dst1[j] = (*(unsigned short *)(dst + i + 1)) * 2 /dst[2] ;
                        j++;
                        i = i + 3;
                    }
                    else
                    {
#ifdef IR_DEBUG
                        printf("*%d ",*(unsigned short *)(dst + i + 2));
#endif
                        dst1[j] = (*(unsigned short *)(dst + i + 2)) * 2 /dst[2] ;
                        j++;
                        i = i + 4;
                    }
                }
                else if(*(dst + i) > 10)
                {
                    value += *(dst + i);
                    i++;
                }
                else
                {
                    i++;
                }
            } //while(i<=IR_DATA_LENGTH)

            if(j>=71*3)
            {
                printf("\n\n*** bad ir code\n");
                offset += IR_DATA_LENGTH;
                if(offset == IR_DATA_LENGTH*IR_DATA_NUM)
                    offset = 0;
                continue;
            }

            dst1[j] = dst1[j-2];//FIXME
            //#ifdef IR_DEBUG
            //printf("j %d====\n\n",j);
#if 1
            for(j=0;j<215 ;j++)
            {
                printf("->%d",dst1[j]);
                arr[j]=dst1[j];
                if (j%20 == 1)
                {
                    printf("\n");
                }
            }
        
            for(m=0;m<8;m++) //panduan
            {
                if((40<arr[x]<43)&&(arr[y]==44))
                    printf("0 ");
                if((40<arr[x]<43)&&(arr[y]==129))
                {
                    printf("1 ");
                    addr+=(pow(2,z));
                }
                z--;
                x+=2;
                y+=2;
            }
            printf("addr:%d\n",addr);
    
            if(addr==128)
            {
                new_ip=IR_bijiao(arr);
                if(new_ip!=0)
                {
                    printf("bijiao  new_ip is :%d\n",new_ip);
                    time=0;
                }
            }
            printf("arr:->%d->%d->%d\n",arr[0],arr[1],arr[2]);

            if((arr[0]<682)&&(arr[0]>678)) // chu luanma
            {
                if(((arr[1]<176)&&(arr[1]>173))&&((arr[2]<683)&&(arr[2]>678)))
                {
                    printf("luan ma chu li !\n");
                    goto luan;
                }
            }
#endif
#endif
            offset += IR_DATA_LENGTH;
            if(offset == IR_DATA_LENGTH*IR_DATA_NUM)
                offset = 0;
#if 0
            for(i = 0; i < 3600; i++) {
                if(*(dst + i)>0)
                    value += *(dst + i);
                else 
                    break;	
            }
            //printf("value:%d\n",value);
            if(value < 50000) {
                //printf("repeat=\n");
                value =0;
                continue; //not handle repeat code
            }
#endif
                value =0;
            } else {
                offset += IR_DATA_LENGTH;
                if(offset == IR_DATA_LENGTH*IR_DATA_NUM)
                    offset = 0;
                usleep(10000);
                continue;
            }
#if 1
            //printf("send start\n");
            int len=0;
#ifdef IR_DEBUG
            int count;
            for (count=0; count<needSend; ++count)
            {
            //    printf("%d\n",dst1[count]);
                printf("0x%x ", dst1[count]);
                if (count%20 == 1)
                {
                    printf("\n");
                }
            }
            // printf("dst1 : %s\n",dst1);

#endif
#if 1
        printf("addr:%d\n",addr);
        
        if(addr!=128)
        {
            printf("addr:%d\n",addr);
            strcpy(NEW,share_mem->sm_eth_setting.strEthIp);
            sscanf(NEW,"192.168.1.%d",&new_multicast);
            bzero(NEW,sizeof(NEW));
            printf("*************************\n"); 
        
            if  (new_multicast!=old_multicast)
            {
                printf("new_multicast:%d,old_multicast:%d\n",new_multicast,old_multicast);
                printf("new_multicast!=old_multicast!  goto socket!\n");
                old_multicast=new_multicast;
                close(sock_cli);
                goto ReSocket;
            }
            else
            {
                printf(" old_multicast=new_multicast  !!! \n");

                printf("new_multicast:%d,old_multicast:%d\n",new_multicast,old_multicast);
                    //  len=sendto(sock_cli,tt,strlen(tt),0);
                len=sendto(sock_cli, dst1, needSend,0,(struct sockaddr *)&server_addr, sizeof(server_addr));
                if(len <= 0)
                {
                    perror("ERRPR");
                    printf("re-connect!\n");
                    close(sock_cli);
                    sleep(2);
                    goto ReSocket;
                }
                printf("send over\n");
                if(len != needSend)
                printf("actual send len:%d\n",len);
            }
        } /*if(addr!=128)*/
#endif
luan:
#endif
        usleep(10000);

    } //while

    free(dst);
    //close(sock_cli);

    return 0;
}
#else
static SL_POINTER  send_ir(SL_POINTER Args)
{
    ///sockfd
    struct ifreq if0;
    int sock_cli;
    static int offset = 0;
    static int value = 0;
    static int cnt = 0;

    int needSend=sizeof(Node);
    unsigned char *dst=(unsigned char*)malloc(needSend);
    int i;

    init_dsp_ir();
#if 1
try_again:
    sock_cli = socket(AF_INET,SOCK_STREAM, 0);

    if(netInterface == INTERFACE_WLAN0)
    {
        strncpy(if0.ifr_name,"wlan0",IFNAMSIZ);
        if(ioctl(sock_cli,SIOCGIFHWADDR,&if0)<0)
        {
            printf("ioctl SIOCGIFHWADDR error\n");
            return -1;
        }
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(IR_CLIENT_PORT);
    if(netInterface == INTERFACE_WLAN0)
        servaddr.sin_addr.s_addr = inet_addr("10.10.1.1");
    else
        servaddr.sin_addr.s_addr = inet_addr("192.168.1.3");

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        close(sock_cli);
        sleep(2);
        goto try_again;
        //exit(1);
    }

#endif
    while(1)
    {

        if((*(dsp_ir_start_addr_va + offset)) != 0)
        {
            usleep(100000);//FIXME let m0 capture entire data must > 50ms
            memcpy(dst, (unsigned char *)(dsp_ir_start_addr_va + offset),IR_DATA_LENGTH);
#ifdef IR_DEBUG
            i=0;
            while(i<=IR_DATA_LENGTH)	
            {
                //	printf("%d :%x\n",i,*(dst + i));
                if(*(dst + i) == 0xff)
                {
                    //printf("line:%d\n",__LINE__);
                    if(value > 0)
                    {
                        printf("-%d ",value);
                        value = 0;
                    }
                    if(i%2)
                    {
                        printf("*%d ",*(unsigned short *)(dst + i + 1));
                        i = i + 3;
                    }
                    else
                    {

                        printf("*%d ",*(unsigned short *)(dst + i + 2));

                        i = i + 4;
                    }
                }
                else if(*(dst + i) > 10)
                {
                    //printf("line:%d\n",__LINE__);
                    value += *(dst + i);
                    i++;
                }
                else
                {
                    i++;
                }
            }
            printf("=====\n\n");
#endif
            memset((unsigned char *)(dsp_ir_start_addr_va + offset),0x00,IR_DATA_LENGTH); //FIXME

            offset += IR_DATA_LENGTH;
            if(offset == IR_DATA_LENGTH*IR_DATA_NUM)
                offset = 0;

#if 0
            for(i = 0; i < 3600; i++) {
                if(*(dst + i)>0)
                    value += *(dst + i);
                else 
                    break;	
            }
            //	printf("value:%d\n",value);
            if(value < 50000) {
                //			printf("repeat=\n");
                value =0;
                continue; //not handle repeat code
            }
#endif
            value =0;
        } else {
            offset += IR_DATA_LENGTH;
            if(offset == IR_DATA_LENGTH*IR_DATA_NUM)
                offset = 0;
            usleep(10000);
            continue;
        }
#if 1
        printf("send start\n");
        int len=0;
        len=send(sock_cli, dst, needSend,0);
        if(len <= 0)
        {
            perror("ERRPR");
            printf("re-connect!\n");
            close(sock_cli);
            sleep(2);
            goto try_again;
        }
        printf("send over\n");
        if(len != needSend)
            printf("actual send len:%d\n",len);
#endif
#ifdef IR_DEBUG
        //printf("\ncnt:%d\n",cnt);
        //cnt ++;
#endif
        usleep(10000);
    }
    free(dst);
    close(sock_cli);
    printf("Send over!!!\n");

    return 0;
}
#endif