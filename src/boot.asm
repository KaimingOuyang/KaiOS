
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

