/*
 * Helper library for ftrace hooking kernel functions
 * Author: Harvey Phillips (xcellerator@gmx.com)
 * License: GPL
 * */
#ifndef __FTRACE_HELPER__
#define __FTRACE_HELPER__

#include <linux/ftrace.h>
#include <linux/version.h>

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
// #define PTREGS_SYSCALL_STUBS 1
#endif


#define HOOK(_hook, _orig)   \
{                   \
    .function = (_hook),        \
    .original = (_orig),        \
}

/* We pack all the information we need (name, hooking function, original function)
 * into this struct. This makes is easier for setting up the hook and just passing
 * the entire struct off to fh_install_hook() later on.
 * */
struct ftrace_hook {
    void *function;
    void *original;

    unsigned long address;
    struct ftrace_ops ops;
};


unsigned long resolve_sym(const char *sym_name);
int fh_install_hook(struct ftrace_hook *hook);
void fh_remove_hook(struct ftrace_hook *hook);
int fh_install_hooks(struct ftrace_hook *hooks, size_t count);
void fh_remove_hooks(struct ftrace_hook *hook, size_t count);

#endif
