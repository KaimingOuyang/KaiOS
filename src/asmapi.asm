global printf,putchar
section .text
printf:
    mov eax, [esp + 4]
    mov ebx, 1
    int 0x40
    ret

putchar:
    mov eax, [esp + 4]
    mov ebx, 2
    int 0x40
    ret
