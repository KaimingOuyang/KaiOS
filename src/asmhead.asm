org 0xbe00

TEXT_MODE EQU 0x10a ;132*43 text mode
VRAM EQU 0x0ff8 ;video buffer address

    jmp sc80_25

GDTR:
align 8
    dw 23
    dd 0

GDT:
align 8
    resb 8
    dw 0xffff,0x0000,0x9200,0x00cf ; data
    dw 0xffff,0x0000,0x9a00,0x00cf ; code

[bits 16]
set_screen:
    ;mov ax,0x4f02
    ;mov bx,0x10c
    ;int 10
    ;cmp ah,1
    ;je sc80_25
    ;jmp close_pic
sc80_25:
    mov al,3; 80*25 text mode
    mov ah,0
    int 0x10
    ;mov dword [VRAM],0xb8000

close_pic:
    mov al,0xff
    out 0x21,al
    nop
    out 0xa1,al
    cli

enable_A20:
    call wait_keyboard
    mov	al,0xd1
    out	0x64,al
    call wait_keyboard
    mov	al,0xdf			; enable a20
    out	0x60,al
    call wait_keyboard
    jmp protected_mode

wait_keyboard:
    in	al,0x64
    and	al,0x02
    jnz	wait_keyboard
    ret
    ;in al, 0x92 ; another way to enable A20
    ;or al, 2
    ;out 0x92, al
    ;mov ax,0xb800
    ;mov gs,ax
    ;mov ax,0x0241
    ;mov word [gs:0],ax

protected_mode:
    mov ax,0xb800
    mov gs,ax
    mov al,[milk+2]
    mov ah,02
    mov [gs:2],ax
    xor eax,eax
    mov ax,cs
    mov ds,ax
    mov ss,ax
    mov es,ax

    mov ax,ds
    shl eax,4
    add eax,GDT
    mov dword [GDTR+2],eax

    lgdt [cs:GDTR]

    mov eax,cr0
    and eax,0x7fffffff
    or	eax,1
	mov	cr0,eax
    jmp reload_register
reload_register:

    xor ax,ax
    mov ax,8
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    jmp dword 2*8:0x0000eb30
    ;mov esp,0x7c00
    ;mov ax,0x0241
    ;mov [ds:0xb8000],ax

    mov eax,0x40000 ;256KB may be modified
    mov edi,0x100000 ;destination
    mov esi,milk;source
    add esi,0x1000

    call memcpy
    jmp dword 2*8:0x00101b30

memcpy:
    mov ebx,0
cpyloop:
    mov ecx,[esi]
    mov [edi],ecx
    add ebx,4
    add edi,4
    add esi,4
    cmp ebx,eax
    jne cpyloop
    ret


    resb 512 - ($ - $$)
milk:
