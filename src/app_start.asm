
section .text
global _start
extern main
_start:
    ;jmp $
    call main
    mov ebx,3
    int 0x40
