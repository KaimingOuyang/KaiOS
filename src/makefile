# KaiOS makefile
CC = i686-elf-gcc
OBJS = fifo.o gdt_idt.o interrupts.o kernel.o keyboard.o memory.o \
	   shell.o string.o task.o tty.o boot.o asmfunc.o io.o
BINFLAGS = -T linker.ld -std=gnu99 -O2 -ffreestanding -nostdlib -lgcc
OBJFLAGS = -c -ffreestanding -O2 -Wall -Wextra -std=gnu99 -I .
NASM = nasm
NASMFLAGS = -felf32
COMFILE = makefile linker.ld


milk.bin: $(OBJS) $(COMFILE)
	$(CC)  $(BINFLAGS) -o milk.bin $(OBJS)

%.o:%.c %.h $(COMFILE)
	$(CC) $(OBJFLAGS) $*.c

%.o:%.asm %.h $(COMFILE)
	$(NASM) $(NASMFLAGS) -o $*.o $*.asm

boot.o:boot.asm $(COMFILE)
	$(NASM) $(NASMFLAGS) -o $*.o $*.asm

mbr.bin:mbr.asm $(COMFILE)
	$(NASM) -fbin -o mbr.bin mbr.asm

asmhead.bin:asmhead.asm $(COMFILE)
	$(NASM) -fbin -o asmhead.bin asmhead.asm

milk.img:mbr.bin asmhead.bin milk.bin $(COMFILE)
	mk.mbr mbr.bin milk.img
	mkfs.fat12 milk.img asmhead.bin milk.bin

.PHONY:run clean

run:milk.img
	qemu-system-i386 -m 256M -vga vmware -fda milk.img

clean:
	@rm $(OBJS)
	@rm milk.bin
	@rm asmhead.bin
	@rm mbr.bin



