# KaiOS makefile
CC = i686-elf-gcc
OBJS = fifo.o gdt_idt.o interrupts.o kernel.o keyboard.o memory.o \
	   shell.o string.o task.o tty.o boot.o asmfunc.o io.o
BINFLAGS = -T linker.ld -std=gnu99 -O2 -ffreestanding -nostdlib -lgcc
OBJFLAGS = -c -ffreestanding -O2 -Wall -Wextra -std=gnu99 -I .
NASM = nasm
NASMFLAGS = -felf32
COMFILE = makefile linker.ld

kaios.iso:kaios.bin
	grub-mkrescue --modules="/boot/grub/i386-pc/vga_text.mod /boot/grub/i386-pc/vga.mod /boot/grub/i386-pc/gfxterm.mod" -o kaios.iso root

kaios.bin: $(OBJS) $(COMFILE)
	$(CC)  $(BINFLAGS) -o kaios.bin $(OBJS)
	@cp -f kaios.bin root/boot/

%.o:%.c %.h $(COMFILE)
	$(CC) $(OBJFLAGS) $*.c

%.o:%.asm %.h $(COMFILE)
	$(NASM) $(NASMFLAGS) -o $*.o $*.asm

boot.o:boot.asm $(COMFILE)
	$(NASM) $(NASMFLAGS) -o $*.o $*.asm

.PHONY:run clean

run:kaios.iso
	qemu-system-i386 -m 1G -vga vmware -cdrom kaios.iso

clean:
	@rm $(OBJS)
	@rm kaios.bin
	@rm root/boot/kaios.bin
	@rm kaios.iso



