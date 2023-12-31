ENTRY(_start)
OUTPUT_FORMAT("binary")

SECTIONS
{
    .text.start : {
        *(.text.start)
    }

    # want some spacing
    . = . + 128;
    . = ALIGN(16);
    .text : {
        *(.text)
    }

    .rodata : ALIGN(16) {
        *(.rodata)
    }

    .data : ALIGN(16) {
        _DATA_START_ = .;
        *(.data)
        _DATA_END_ = .;
    }

    .bss : ALIGN(16) {
        _BSS_START_ = .;
        *(.bss)
        _BSS_END_ = .;
    }

    /DISCARD/ : { *(.note*) }
}
