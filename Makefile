PROG = BOOTX64.EFI
CC = x86_64-w64-mingw32-gcc
OBJS = efi_main.o file.o elf.o memory.o gop.o errors.o memory.o

$(PROG): $(OBJS) libefi/libefi.a
	$(CC) -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main -o $(PROG) $^  -Llibefi/ -l:libefi.a

%.o: %.c
	$(CC) -O2 -ffreestanding -I libefi/include/ -I include/ -I include/amd64 -c -o $@ $<

libefi/libefi.a:
	make -C libefi/

.PHONY: clean run

clean:
	rm -rf *.o ext2/*.o $(PROG) $(IMG)

	make clean -C libefi/
