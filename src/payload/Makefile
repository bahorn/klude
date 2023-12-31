KBASE=/usr/src/linux-headers-$(KVERSION)
# Mostly ripped from building a normal module
# Potentially more flags need to be added here, but ignoring for now.
CFLAGS_BUILD=-O2 \
	   -nostdinc \
	   -isystem /usr/lib/gcc/x86_64-linux-gnu/11/include \
	   -I$(KBASE)/include \
	   -I$(KBASE)/arch/x86/include/ \
	   -I$(KBASE)/arch/x86/include/generated \
	   -I$(KBASE)/arch/x86/include/generated/uapi \
	   -I$(KBASE)/arch/x86/include/uapi \
	   -I$(KBASE)/include/uapi \
	   -I$(KBASE)/include/generated/uapi \
	   -I$(KBASE)/ubuntu/include \
	   -I ./src/include/ \
	   -include $(KBASE)/include/linux/compiler-version.h \
	   -include $(KBASE)/include/linux/kconfig.h \
	   -fPIE \
	   -fno-plt \
	   -nostdlib \
	   -nolibc \
	   -DCONFIG_X86_X32_ABI \
	   -DMODULE \
	   -D__KERNEL__ \
	   -Wall \
	   -Wundef \
	   -Werror=strict-prototypes \
	   -Wno-trigraphs \
	   -fno-strict-aliasing \
	   -fno-common \
	   -fshort-wchar \
	   -Werror=implicit-function-declaration \
	   -Werror=implicit-int \
	   -Werror=return-type \
	   -Wno-format-security \
	   -std=gnu99 \
	   -mno-sse \
	   -mno-mmx \
	   -mno-sse2 \
	   -mno-3dnow \
	   -mno-avx \
	   -m64 \
	   -falign-jumps=1 \
	   -falign-loops=1 \
	   -mno-80387 \
	   -mno-fp-ret-in-387 \
	   -mpreferred-stack-boundary=3 \
	   -mskip-rax-setup \
	   -mtune=generic \
	   -mno-red-zone \
	   -Wno-sign-compare \
	   -fno-asynchronous-unwind-tables \
	   -fno-jump-tables \
	   -mharden-sls=all \
	   -fno-delete-null-pointer-checks \
	   -Wno-frame-address \
	   -Wno-format-truncation \
	   -Wno-format-overflow \
	   -Wno-address-of-packed-member \
	   -fno-allow-store-data-races \
	   -Wframe-larger-than=1024 \
	   -fstack-protector-strong \
	   -Wimplicit-fallthrough=5 \
	   -Wno-main \
	   -Wno-unused-but-set-variable \
	   -Wno-unused-const-variable \
	   -fno-omit-frame-pointer \
	   -fno-optimize-sibling-calls \
	   -fno-stack-clash-protection \
	   -mrecord-mcount \
	   -mfentry \
	   -DCC_USING_FENTRY \
	   -Wdeclaration-after-statement \
	   -Wvla \
	   -Wno-pointer-sign \
	   -Wno-stringop-truncation \
	   -Wno-zero-length-bounds \
	   -Wno-array-bounds \
	   -Wno-stringop-overflow \
	   -Wno-restrict \
	   -Wno-maybe-uninitialized \
	   -Wno-alloc-size-larger-than \
	   -fno-strict-overflow \
	   -fno-stack-check \
	   -fconserve-stack \
	   -Werror=date-time \
	   -Werror=incompatible-pointer-types \
	   -Werror=designated-init \
	   -Wno-packed-not-aligned \
	   -fsanitize=bounds \
	   -fsanitize=shift \
	   -fsanitize=bool \
	   -fsanitize=enum


LDFLAGS=-T ./scripts/static-pie.ld

SRC_DIR=./src
OBJ_DIR=./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))


build: all.o stubgen-build
	$(LD) $(LDFLAGS) -o main.bin ./all.o ./scripts/stubgen/stub_lib
	strip main.bin
	xxd -i main.bin > ./embedded.h

all.o: $(OBJ_FILES)
	$(LD) -r -o all.o $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS_BUILD) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Generates a stub file to preserve the relocations
stubgen-build:
	python3 ./scripts/stubgen/gen.py ./scripts/stubgen/kallsyms | sort | uniq > ./scripts/stubgen/stubs.c
	$(CC) -Wno-builtin-declaration-mismatch -fno-plt -nostdlib -fPIE -shared -o ./scripts/stubgen/stub_lib ./scripts/stubgen/stubs.c

clean:
	-rm -r $(OBJ_DIR)
	-rm main.bin
	-rm embedded.h
	-rm all.o
