#include "mem.h"
#include "ftrace_helper.h"


void *vmalloc_node_range(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			const void *caller)
{
    typedef void * (*__vmalloc_node_range_t)(unsigned long size, unsigned long align,
			unsigned long start, unsigned long end, gfp_t gfp_mask,
			pgprot_t prot, unsigned long vm_flags, int node,
			const void *caller);
    __vmalloc_node_range_t __vmalloc_node_range = (__vmalloc_node_range_t) resolve_sym("__vmalloc_node_range");

    return __vmalloc_node_range(size, align, start, end, gfp_mask, prot, vm_flags, node, caller);
}

void *__vmalloc_node(unsigned long size, unsigned long align,
                gfp_t gfp_mask, int node, const void *caller)
{
    return vmalloc_node_range(size, align, VMALLOC_START, VMALLOC_END,
                gfp_mask, PAGE_KERNEL, 0, node, caller);
}

void *__vmalloc_node_exec(unsigned long size, unsigned long align,
                gfp_t gfp_mask, int node, const void *caller)
{
    return vmalloc_node_range(size, align, VMALLOC_START, VMALLOC_END,
                gfp_mask, PAGE_KERNEL_EXEC, 0, node, caller);
}


void *__vmalloc(unsigned long size, gfp_t gfp_mask)
{
    return __vmalloc_node(size, 1, gfp_mask, NUMA_NO_NODE,
                __builtin_return_address(0));
}

void *__vmalloc_exec(unsigned long size, gfp_t gfp_mask)
{
    return __vmalloc_node_exec(size, 1, gfp_mask, NUMA_NO_NODE,
                __builtin_return_address(0));
}
