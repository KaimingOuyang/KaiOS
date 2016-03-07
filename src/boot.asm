# Multiboot header

MAGIC EQU 0x1BADB002
MALIGN EQU 1 << 0
MEMINFO EQU 1 << 1
FLAGS EQU MALIGN | MEMINFO
CHECKSUM EQU -(FLAGS + MAGIC)

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM


section .data
gdt
    resb 8
    DW		0xffff,0x0000,0x9200,0x00cf
    DW		0xffff,0x0000,0x9a00,0x00cf
gdtr DW 3*8-1 ; For limit storage
     DD gdt ; For base storage


section .bootstrap_stack
align 4
stack_top:
    dd 10 << 16

section .text
global _start
extern KaiOS_main

_start:

    mov esp,[stack_top]
    call reloadSegments
    jmp KaiOS_main
	cli
_halt:
	hlt
	jmp _halt

reloadSegments:
    LGDT  [gdtr]
   ; Reload CS register containing code selector:
   JMP   0x10:reload_CS ; 0x08 points at the new code selector
reload_CS:
   ; Reload data segment registers:

   MOV   AX, 0x08 ; 0x10 points at the new data selector
   MOV   DS, AX
   MOV   ES, AX
   MOV   FS, AX
   MOV   GS, AX
   MOV   SS, AX
   RET
