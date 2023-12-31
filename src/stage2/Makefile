CFLAGS=-fPIE -fno-builtin -nostdlib -fno-plt -O2 -I ./src/include
LDFLAGS=-T ./scripts/linker.lds

build:
	$(CC) $(CFLAGS) -c ./src/main.c -o ./all.o
	$(LD) $(LDFLAGS) ./all.o -o stage2.bin

clean:
	-rm all.o stage2.bin
