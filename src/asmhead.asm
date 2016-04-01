
VRAM EQU 0x0ff8
    org 0xbe00
[bits 16]
sc80_25:
    mov al,3; 80*25 text mode
    mov ah,0

    int 0x10

    mov dword [VRAM],0xb8000

close_pic:
    mov al,0xff
    out 0x21,al
    nop
    out 0xa1,al
    cli

enable_A20:
    in al, 0x92 ; another way to enable A20
    or al, 2
    out 0x92, al

protected_mode:
    lgdt [GDTR]
    mov eax,cr0
    and eax,0x7fffffff
    or	eax,0x00000001
    nop
    nop
    nop
	mov	cr0,eax
    jmp 2*8:reload_register
[bits 32]
reload_register:
    mov ax,8
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    mov eax,[milk+0x18]
    mov [MILK_ADDR],eax

    mov eax,0x40000 ;256KB may be modified
    mov edi,0x100000 ;destination
    mov esi,milk;source
    add esi,0x1000
    call memcpy
begin:
    jmp far [MILK_ADDR]

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

MILK_ADDR:
    dd 0
    dw 16

alignb 8
GDTR:
    dw 23
    dd GDT

GDT:
    resb 8
    dw 0xffff,0x0000,0x9200,0x00cf ; data
    dw 0xffff,0x0000,0x9a00,0x00cf ; code
    alignb 512

milk:
