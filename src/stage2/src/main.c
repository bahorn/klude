/* SHELF loader.
 * 
 * Do not bother trying to split this into multiple files.
 *
 * It is a nightmare...
 */
#include <elf.h>
#include <stddef.h>
#include <stdbool.h>

#include "./include/reloc.h"
#include "../../payload/embedded.h"

#define PAGE_SIZE 4096

addr_holder kallsyms_addr_holder = {
    0x4142434445464748,
    (kallsyms_lookup_name_t) 0x6162636465666769
};


kallsyms_lookup_name_t kallsyms_lookup_name;
_printk_t printk;
vmalloc_t vmalloc;
set_memory_x_t set_memory_x;
set_memory_rw_t set_memory_rw;

/* Util Functions */ 

/* Basic stolen strcmp implementation:
 * https://stackoverflow.com/questions/34873209/implementation-of-strcmp
 */
int strcmp(const char *s1, const char *s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = (char *)dest;
    char *s = (char *)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}


void *memset(void *s, int c, size_t n)
{
    char *d = (char *)s;
    for (size_t i = 0; i < n; i++) {
        d[i] = 0;
    }
    return s;
}

/* Maps a size to the number of pages */
size_t get_n_pages(size_t n)
{
    size_t i = (n / PAGE_SIZE);
    if ((n % PAGE_SIZE) > 0) {
        i += 1;
    }
    return i;
}


/* ELF LOADER */
char *strtab_p = NULL;
char *dynstr_p = NULL;
Elf64_Sym *dynsym_p = NULL;


char *find_strtab(void *b)
{
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) b;
    Elf64_Shdr *_shdr = NULL;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        _shdr = (Elf64_Shdr *)(b + ehdr->e_shoff + ehdr->e_shentsize * i);
        switch (_shdr->sh_type) {
            case SHT_STRTAB:
                if (_shdr->sh_flags != 0x00) {
                    break;
                }

                char *strtab = (char *) (b + _shdr->sh_offset);
                return strtab;
            default:
                break;
        }
    }

    return NULL;
}


char *find_dynstr(void *b)
{
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) b;
    Elf64_Shdr *_shdr = NULL;

    if (strtab_p == NULL) {
        return NULL;
    }

    for (int i = 0; i < ehdr->e_shnum; i++) {
        _shdr = (Elf64_Shdr *)(b + ehdr->e_shoff + ehdr->e_shentsize * i);
        switch (_shdr->sh_type) {
            case SHT_STRTAB:
                if (strcmp(strtab_p + _shdr->sh_name, ".dynstr") != 0) {
                    break;
                }
                char *dynstrtab = (char *) (b + _shdr->sh_offset);
                return dynstrtab;
            default:
                break;
        }
    }

    return NULL;
}


/* Find the dynamic symbols section */
Elf64_Sym *find_dynsyms(void *b)
{
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) b;
    Elf64_Shdr *_shdr = NULL;

    if (strtab_p == NULL || dynstr_p == NULL) {
        return NULL;
    }

    for (int i = 0; i < ehdr->e_shnum; i++) {
        _shdr = (Elf64_Shdr *)(b + ehdr->e_shoff + ehdr->e_shentsize * i);
        switch (_shdr->sh_type) {
            case SHT_DYNSYM:
                return (Elf64_Sym *)(b + _shdr->sh_offset);
                break;
            default:
                break;
        }
    }
    return NULL;
}


/* Fill in global variables for useful sections */
bool get_useful_sections(void *b)
{
    strtab_p = find_strtab(b);
    dynstr_p = find_dynstr(b);
    dynsym_p = find_dynsyms(b);
   
    return (strtab_p != NULL && dynstr_p != NULL && dynsym_p != NULL);
}


/* Apply a single relocation */
bool apply_reloc(void *target, Elf64_Rela *relocation)
{
    int idx = ELF64_R_SYM(relocation->r_info);
    int type = ELF64_R_TYPE(relocation->r_info);
    int64_t r_addend = relocation->r_addend;


    char *symbol_name = (char *)(dynstr_p + dynsym_p[idx].st_name);
    unsigned long sym_addr = kallsyms_lookup_name(symbol_name);

    switch (type) {
        case R_X86_64_GLOB_DAT:
            printk("> R_X86_64_GLOB_DAT\n");
            unsigned long *to_patch = \
                    (unsigned long *)(target + relocation->r_offset);
            *to_patch = sym_addr + r_addend;
            return true;
        default:
            printk("unknown relocation type\n");
            return false;
    }
}


/* Apply all the relocations */
bool apply_relocs(void *target, Elf64_Rela *relocations, int count)
{
    for (int i = 0; i < count; i++) {
        if (!apply_reloc(target, &relocations[i])) {
            return false;
        }
    }
    return true;
}


/* Apply all the relocations in the ELF */
bool apply_all_relocs(void *b, void *target)
{
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) b;
    Elf64_Shdr *_shdr = NULL;
    Elf64_Rela *relocation = NULL;
    for (int i = 0; i < ehdr->e_shnum; i++) {
        _shdr = (Elf64_Shdr *)(b + ehdr->e_shoff + ehdr->e_shentsize * i);
        switch (_shdr->sh_type) {
            case SHT_RELA:
                int count = _shdr->sh_size / _shdr->sh_entsize;
                relocation = (Elf64_Rela *)(b + _shdr->sh_offset);
                return apply_relocs(target, relocation, count);
                break;
            default:
                break;
        }
    }
    return false;
}


/* Process the single program header that should exist in the SHELF */
void *process_segment(void *elf, size_t len)
{
    void *res = NULL;
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) elf;
    Elf64_Phdr *_phdr = NULL;

    /* This loader is basic, this shouldn't happen */
    if (ehdr->e_phnum != 1) {
        return NULL;
    }

    /* Allocate the pages */
    _phdr = (Elf64_Phdr *)(elf + ehdr->e_phoff);

    if (_phdr->p_filesz > _phdr->p_memsz) {
        return NULL;
    }

    res = vmalloc(_phdr->p_memsz);
    if (res == NULL) {
        return res;
    }

    printk("Allocated page: %p\n", res);

    memset(res, 0, _phdr->p_memsz);
    memcpy(res, elf + _phdr->p_offset, _phdr->p_filesz);
    set_memory_x((unsigned long) res, get_n_pages(len));
    //set_memory_rw((unsigned long) res, get_n_pages(len));

    return res;
}


/* Apply the relocations */
bool do_relocation(void *elf, void *code)
{
    if (!get_useful_sections(elf)) {
        return false;
    }
    return apply_all_relocs(elf, code);
}


/* Run */
void run(void *elf, void *code)
{
    typedef void (*start_t)(void);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) elf;
    printk("Entrypoint: %p\n", code + ehdr->e_entry);
    start_t start = (start_t)(code + ehdr->e_entry);
    start();
}

/* Load and run the ELF */
void run_elf(void *elf, size_t len)
{
    /* Get the code loaded and relocated */
    void *loaded = process_segment(elf, len);
    if (!do_relocation(elf, loaded)) {
        printk("reloc failed?\n");
        return;
    }
    /* Call the entry point */
    run(elf, loaded);
}


/* Entrypoint */
__attribute__ ((section(".text.start")))
void _start()
{
    /* Resolving functions we need */
    kallsyms_lookup_name = kallsyms_addr_holder.kallsyms_addr;
    vmalloc = (vmalloc_t) kallsyms_lookup_name("vmalloc");
    printk = (_printk_t) kallsyms_lookup_name("_printk");
    set_memory_x = (set_memory_x_t) kallsyms_lookup_name("set_memory_x");
    set_memory_rw = (set_memory_rw_t) kallsyms_lookup_name("set_memory_rw");

    /* Run the SHELF now the rest of the code can use the required symbols */
    printk("Running kSHELF\n");
    run_elf(&main_bin, main_bin_len);
}
