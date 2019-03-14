#ifndef _NALU_H_
#define _NALU_H_

#include "rtp.h"

typedef struct  
{  
    int startcodeprefix_len;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)  
    unsigned len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)  
    unsigned max_size;            //! Nal Unit Buffer size  
    int forbidden_bit;            //! should be always FALSE  
    int nal_reference_idc;        //! NALU_PRIORITY_xxxx  
    int nal_unit_type;            //! NALU_TYPE_xxxx      
    char *buf;                    //! contains the first byte followed by the EBSP  
    unsigned short lost_packets;  //! true, if packet loss is detected  
} NALU_t;  
  
typedef struct {  
    //byte 0  
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;
  
} NALU_HEADER; /**//* 1 BYTES */  

typedef struct {  
    //byte 0  
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;
} FU_INDICATOR; /**//* 1 BYTES */  
  
typedef struct {  
    //byte 0  
    unsigned char TYPE:5;
    unsigned char R:1;
    unsigned char E:1;
    unsigned char S:1;
} FU_HEADER; /**//* 1 BYTES */  


//FILE *bits = NULL;                //!< the bit stream file  
static int FindStartCode2 (unsigned char *Buf);//查找开始字符0x000001  
static int FindStartCode3 (unsigned char *Buf);//查找开始字符0x00000001  
//static bool flag = true;  
static int info2=0, info3=0;
RTP_FIXED_HEADER        *rtp_hdr;  
  
NALU_HEADER     *nalu_hdr;  
FU_INDICATOR    *fu_ind;  
FU_HEADER       *fu_hdr;  

extern FILE *bits;


void FreeNALU(NALU_t *n);
void OpenBitstreamFile (char *fn);
int GetAnnexbNALU(NALU_t *nalu);
void dump(NALU_t *n);
NALU_t *AllocNALU(int buffersize); 


#endif
