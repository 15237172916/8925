#ifndef _SL_RESULT_FDIP_DSP_H_
#define _SL_RESULT_FDIP_DSP_H_

#include <sl_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FDIP DSP OUTPUT DATA FORMAT */
typedef struct __SLFD_DSP_COORD{
    SL_S32          left;    
    SL_S32          top; 
    SL_S32          right; 
    SL_S32          bottom; 
}SLFD_DSP_COORD_S;

#ifdef __cplusplus
}
#endif

#endif /* _SL_RESULT_FDIP_DSP_H_ */

