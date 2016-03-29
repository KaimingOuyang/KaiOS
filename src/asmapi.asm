global printf

section .text
printf:
    mov eax, [esp + 4]
    mov ebx, 1
    int 40
    ret

putchar:
    mov eax, [esp + 4]
    mov ebx, 2
    int 40
    ret
