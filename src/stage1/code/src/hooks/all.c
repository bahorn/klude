#include "random_dev.h"


int install_hooks(void)
{
    int err;
    err = install_random_hooks();
    if (err) {
        return err;
    }

    return 0;
}


void remove_hooks(void)
{
    remove_random_hooks();
}
