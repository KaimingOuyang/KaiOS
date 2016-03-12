
MAGIC EQU 0x1BADB002
MALIGN EQU 1 << 0
MEMINFO EQU 1 << 1
GRAPHMODE EQU 1 << 2
FLAGS EQU MALIGN | MEMINFO | GRAPHMODE
CHECKSUM EQU -(FLAGS + MAGIC)

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
    dd 0,0,0,0,0
    dd 1,40,25,0

section .bootstrap_stack
align 4
stack_top:
    dd 10 << 16

section .text
global _start
extern KaiOS_main
_start:
    mov esp,[stack_top]
    jmp KaiOS_main
    cli
_halt:
	hlt
	jmp _halt

