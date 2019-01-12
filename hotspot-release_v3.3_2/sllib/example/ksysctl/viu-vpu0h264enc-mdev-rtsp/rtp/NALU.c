/*************************************************************************
	> File Name: NALU.c
	> Author: 
	> Mail: 
	> Created Time: Fri 22 Jun 2018 05:39:57 PM CST
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>


#include "NALU.h"


FILE *bits = NULL;                //!< the bit stream file 

void OpenBitstreamFile (char *fn)  
{  
    if (NULL == (bits=fopen(fn, "rb")))  
    {  
        printf("open file error\n");
        exit(0);  
    }  
}



//为NALU_t结构体分配内存空间  
NALU_t *AllocNALU(int buffersize)  
{  
    NALU_t *n;  
  
    if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)  
    {  
        printf("AllocNALU: n");  
        exit(0);  
    }  
  
    n->max_size=buffersize;  
  
    if ((n->buf = (char*)calloc (buffersize, sizeof (char))) == NULL)  
    {  
        free (n);  
        printf ("AllocNALU: n->buf");  
        exit(0);  
    }
  
    return n;  
}  

#if 1
//释放  
void FreeNALU(NALU_t *n)  
{  
    if (n)  
    {  
        if (n->buf)  
        {  
            free(n->buf);  
            n->buf=NULL;  
        }  
        free (n);  
    }  
}  



//这个函数输入为一个NAL结构体，主要功能为得到一个完整的NALU并保存在NALU_t的buf中，获取他的长度，填充F,IDC,TYPE位。  
//并且返回两个开始字符之间间隔的字节数，即包含有前缀的NALU的长度  
int GetAnnexbNALU(NALU_t *nalu)  
{  
    int pos = 0;
    int StartCodeFound, rewind;
    unsigned char *Buf;
  
    if ((Buf = (unsigned char*)calloc(nalu->max_size , sizeof(char))) == NULL)   
    {  
       printf ("GetAnnexbNALU: Could not allocate Buf memory\n");  
    }
    
  
    nalu->startcodeprefix_len=3;//初始化码流序列的开始字符为3个字节  
  
    if (3 != fread (Buf, 1, 3, bits))//从码流中读3个字节  
    {  
        free(Buf);  
        return 0;  
    }
    info2 = FindStartCode2 (Buf);//判断是否为0x000001   
    if(info2 != 1)   
    {  
        //如果不是，再读一个字节  
        if(1 != fread(Buf+3, 1, 1, bits))//读一个字节  
        {  
            free(Buf);  
            return 0;  
        }  
        info3 = FindStartCode3 (Buf);//判断是否为0x00000001  
        if (info3 != 1)//如果不是，返回-1  
        {   
            free(Buf);  
            return -1;  
        }  
        else   
        {  
            //如果是0x00000001,得到开始前缀为4个字节  
            pos = 4;  
            nalu->startcodeprefix_len = 4;  
        }  
    }  
  
    else  
    {  
        //如果是0x000001,得到开始前缀为3个字节  
        nalu->startcodeprefix_len = 3;  
        pos = 3;  
    }  
  
    //查找下一个开始字符的标志位  
    StartCodeFound = 0;  
    info2 = 0;  
    info3 = 0;  
  
    while (!StartCodeFound)  
    {  
        if (feof (bits))//判断是否到了文件尾，文件结束，则返回非0值，否则返回0  
        {  
            nalu->len = (pos-1)-nalu->startcodeprefix_len;  //NALU单元的长度。  
            memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);       
            nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit  
            nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit  
            nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit  
            free(Buf);  
            return pos-1;  
        }  
        Buf[pos++] = fgetc (bits);//读一个字节到BUF中  
        info3 = FindStartCode3(&Buf[pos-4]);//判断是否为0x00000001  
        if(info3 != 1)  
        {  
           info2 = FindStartCode2(&Buf[pos-3]);//判断是否为0x000001  
        }  
              
        StartCodeFound = (info2 == 1 || info3 == 1);  
    }  
  
    // Here, we have found another start code (and read length of startcode bytes more than we should  
    // have.  Hence, go back in the file  
    rewind = (info3 == 1)? -4 : -3;  
  
    if (0 != fseek (bits, rewind, SEEK_CUR))//把文件指针指向前一个NALU的末尾，在当前文件指针位置上偏移 rewind。  
    {  
        free(Buf);  
        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");  
    }  
  
    // Here the Start code, the complete NALU, and the next start code is in the Buf.    
    // The size of Buf is pos, pos+rewind are the number of bytes excluding the next  
    // start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code  
  
    nalu->len = (pos+rewind)-nalu->startcodeprefix_len;    //NALU长度，不包括头部。  
    memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//拷贝一个完整NALU，不拷贝起始前缀0x000001或0x00000001  
    nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit  
    nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit  
    nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit  
    free(Buf);  
  
    return (pos+rewind);//返回两个开始字符之间间隔的字节数，即包含有前缀的NALU的长度  
}  

//输出NALU长度和TYPE  
void dump(NALU_t *n)  
{  
    if (!n)return;  
    //printf("a new nal:");  
    printf(" len: %d  ", n->len);  
    printf("nal_unit_type: %x\n", n->nal_unit_type);  
}  
 
static int FindStartCode2(unsigned char *Buf)  
{  
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0; //判断是否为0x000001,如果是返回1  
    else return 1;  
}  
  
static int FindStartCode3(unsigned char *Buf)  
{  
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;//判断是否为0x00000001,如果是返回1  
    else return 1;  
}  
#endif
