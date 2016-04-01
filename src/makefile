# Milk makefile
CC = i686-elf-gcc
OBJS = fifo.o gdt_idt.o interrupts.o kernel.o keyboard.o memory.o \
	   shell.o string.o task.o tty.o boot.o asmfunc.o io.o api.o
BINFLAGS = -T linker.ld -std=gnu99 -O2 -ffreestanding -nostdlib -lgcc
OBJFLAGS = -c -ffreestanding -O2 -Wall -Wextra -std=gnu99 -I .
NASM = nasm
NASMFLAGS = -felf32
COMFILE = makefile linker.ld


milk.bin: $(OBJS) $(COMFILE)
	$(CC)  $(BINFLAGS) -o milk.bin $(OBJS)

%.o:%.c %.h $(COMFILE)
	$(CC) $(OBJFLAGS) $*.c

%.o:%.c $(COMFILE)
	$(CC) $(OBJFLAGS) $*.c

%.o:%.asm %.h $(COMFILE)
	$(NASM) $(NASMFLAGS) -o $*.o $*.asm


boot.o:boot.asm $(COMFILE)
	$(NASM) $(NASMFLAGS) -o $*.o $*.asm

mbr.bin:mbr.asm $(COMFILE)
	$(NASM) -fbin -o mbr.bin mbr.asm

asmhead.bin:asmhead.asm $(COMFILE)
	$(NASM) -fbin -o asmhead.bin asmhead.asm

milk.img:mbr.bin asmhead.bin milk.bin logo.mk $(COMFILE)
	mk.mbr mbr.bin milk.img
	mkfs.fat12 milk.img asmhead.bin milk.bin logo.mk

## application
app_start.o:app_start.asm $(COMFILE)
	$(NASM) $(NASMFLAGS) -o app_start.o app_start.asm

asmapi.o:asmapi.asm asmapi.h $(COMFILE)
	$(NASM) $(NASMFLAGS) -o asmapi.o asmapi.asm

app_logo.o:app_logo.c $(COMFILE)
	$(CC) -std=gnu99 -I . -c app_logo.c

logo.mk:app_logo.o asmapi.o app_start.o $(COMFILE)
	$(CC) -O2 -ffreestanding -nostdlib -lgcc -T app_linker.ld -o \
	logo.mk app_logo.o asmapi.o app_start.o

.PHONY:run clean

run:milk.img
	qemu-system-i386 -m 256M -vga vmware -fda milk.img

clean:
	@rm *.o
	@rm milk.bin
	@rm asmhead.bin
	@rm mbr.bin
	@rm logo.mk



