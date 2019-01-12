#ifndef _SL_AVMEM_H_
#define _SL_AVMEM_H_

#ifdef __KERNEL__
#include <silan_memory.h>
#endif

typedef void * SLAVMEM_Handle_t;

#define AVMEM_MODULE_NAME  "slavmem"
#define MALLOC_IOCTL_MAGIC  'M'

#define AVMEM_IOCTL_ALLOC_PHYMEM _IO(MALLOC_IOCTL_MAGIC, 0)
#define AVMEM_IOCTL_FREE_PHYMEM  _IO(MALLOC_IOCTL_MAGIC, 1)
typedef struct _SLAVMEM_BlockParam_t {
	u32 pa;
	void *va;  /* Virtual address in kernel space */
	u32 size;
}SLAVMEM_BlockParam_t;

typedef enum {
	ID_DDR0,
	ID_DDR1,
}SLAVMEM_DDR_ID;

typedef struct _SLAVMEM_Ioctl_AllocParam_t {
	SLAVMEM_Handle_t     handle;
	SLAVMEM_DDR_ID       ddrid;
	u32 pa;
	u32 size;
}SLAVMEM_Ioctl_AllocParam_t;

typedef struct _SLAVMEM_Ioctl_FreeParam_t {
	SLAVMEM_Handle_t     handle;
	SLAVMEM_DDR_ID       ddrid;
}SLAVMEM_Ioctl_FreeParam_t;

/*
 * Functions
 */
#ifdef __KERNEL__

typedef struct _SLAVMEM_AllocParam_t {
    ddrId_t ddrid;
	u32 size;
}SLAVMEM_AllockParam_t;

int slavmem_allocBlock(SLAVMEM_AllockParam_t *ap, SLAVMEM_Handle_t *handle,
        SLAVMEM_BlockParam_t *bp);
int slavmem_freeBlock(SLAVMEM_Handle_t handle);
int slavmem_alloc(SLAVMEM_Handle_t *handle, u32 size, int id,
        struct prom_phyaddr *pa);
int slavmem_free(SLAVMEM_Handle_t handle, int id);
int slavmem_getDdrLeastUsers(ddrId_t *ddr_id, bool lowddr_only);
#endif

#endif /* _SL_AVMEM_H_ */

