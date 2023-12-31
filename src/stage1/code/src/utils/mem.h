#ifndef __MEM_UTILS__
#define __MEM_UTILS__

#include <linux/vmalloc.h>

void *vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			const void *caller);
void *__vmalloc_node(unsigned long size, unsigned long align,
                gfp_t gfp_mask, int node, const void *caller);
void *__vmalloc(unsigned long size, gfp_t gfp_mask);
void *__vmalloc_exec(unsigned long size, gfp_t gfp_mask);

#endif
