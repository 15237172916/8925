#ifndef _SL_RESULT_JPU_H_
#define _SL_RESULT_JPU_H_

#include <sl_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JPG_MAX_ELEM (50)

typedef enum _SLJPG_ELEM_TYPE
{
    SLJPG_ELEM_START,
    SLJPG_ELEM_PIC_ORG = SLJPG_ELEM_START,
    SLJPG_ELEM_PIC_FACE,
    SLJPG_ELEM_PIC_COORD,    
    SLJPG_ELEM_END

}SLJPG_ELEM_TYPE_E;

typedef struct _SLJPG_Elem
{
    SLJPG_ELEM_TYPE_E elem_type;
    SL_U32 start;
    SL_U32 size;
    SL_U32 valid;
    SL_U32 rsv;

}SLJPG_Elem_t;

typedef struct _SLJPG_MultiJpgs_HeadInfo
{
    time_t time; /*timestamp*/
    SL_U32 elemCnt;
    SLJPG_Elem_t elem[JPG_MAX_ELEM];
    SL_U32 rsv;

}SLJPG_MultiJpgs_HeadInfo_t;

#ifdef __cplusplus
}
#endif

#endif /* _SL_RESULT_JPU_H_ */
