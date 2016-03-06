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


section .bootstrap_stack
align 4
stack_bottom:
resb 16384
stack_top:

section .text
global _start
_start:
	mov esp,stack_top
	
	extern KaiOS_main
	call KaiOS_main
	
	cli

_halt:
	hlt
	jmp _halt



