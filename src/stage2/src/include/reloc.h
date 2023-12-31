#ifndef __RELOC__
#define __RELOC__
/* Relocation stuff */
#include <stdint.h>

typedef unsigned int gfp_t;
typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
typedef int (*_printk_t)(const char *fmt, ...);
typedef void *(*vmalloc_t)(unsigned long size);
typedef int *(*set_memory_x_t)(unsigned long addr, int numpages);
typedef int *(*set_memory_rw_t)(unsigned long addr, int numpages);

typedef struct addr_holder {
    uint64_t magic;
    kallsyms_lookup_name_t kallsyms_addr;
} __attribute__ ((packed)) addr_holder;

#endif
