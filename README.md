# klude

A prototype kernel SHELF loader.
Implemented as one module that will load another.

The payload module will not show up in the list of modules loaded, and the
loader module can be unloaded.

## Usage

Get a ubuntu 22.04 server box to test on.

### Setup

* Patch `./build-system/Makefile` and set the correct `KVERSION` and run make
  there to build the container for compiling it.
* Update `./Makefile` to use the ip of the test host.


### Running

* run `make`, this will compile the code in the container and copy it to your
  test host
* once done, run `python loader.py` on the test host as root.

Check dmesg, and you'll see output.


## Components

This is a bit overkill in stages, but you run it by just invoking a python
script as root.

### stage0

Python script that uses libc through ctypes to load a kernel module.
Just uses the `init_module()` syscall with an in-memory buffer.

### stage1

A kernel module that acts a in-kernel shellcode loader.
You can pass it a binary linked in a specific way and it will execute it inside
the kernel.

Implemented by a hook for the `/dev/urandom`'s write method.
Mildly interesting, as in recent kernel versions changed it to
`random_write_iter`.

Implemented with the `ftrace_helper.h` lib[2].

Does unset the kernels taint values.
Currently needs to remove some dmesg output.

### stage2

A basic SHELF loader. Single section, mapped RWX. Supports relocations via a
GOT, so payloads can use kernel symbols normally.

### Payload

Currently just the `kill(2)` hook included by xcellerator [1] that grants the
caller root.

For some reason, I have some issues with strings in some sections.
So I had to bypass them when patching the hooking framework.

Use the following program to test:
```c
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    kill(getpid(), 64);
    system("/bin/bash");
}
```

### Combine

Tooling to combine the above components into a single python script that can be
executed.

## Notes

This uses `ftrace_helper.h` by xcellerator[2], with some patches for the different
contexts its used in.
Part of a great tutorial series, check it out.

Didn't find it before I got most of this written, but prior art exists in work 
by Ilya Matveychikov [3][4].
Kopycat does basically what stage1 does, and kmatryoshka seems to be going a 
bit farther with a call to `sys_init_module()` in the kernel.
Though neither of these go out an just parse the ELF by themselves however.

SHELFs are described in [5] by ulexec and Anonymous_.
The linker script for the payload is based off the one in ulexec's SHELF repo. [6]
This was modified a bit to remove the TLS section, add the GOT, and move data to
the main around.

Written against ubuntu 22.04 server, kernel version `5.15.0-91-generic`.
Only supports amd64.

Build system uses docker to install the kernel dependencies.
Go build the container in `build-system` first and update the IP address for
the test system in `Makefile`.

Licensed under the GPL2, as required for kernel stuff.
ulexec didn't include a license for the linker script, but it's derived from
gcc's built in ones so assuming GPL.

## References

* [1] https://xcellerator.github.io/posts/linux_rootkits_03/
* [2] https://github.com/xcellerator/linux_kernel_hacking/blob/master/3_RootkitTechniques/3.3_set_root/ftrace_helper.h
* [3] https://github.com/milabs/kopycat
* [4] https://github.com/milabs/kmatryoshka/tree/master
* [5] https://tmpout.sh/1/10/
* [6] https://github.com/ulexec/SHELF-Loading
