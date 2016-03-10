
section .text
global _load_gdtr,_load_idtr,_out8

_load_gdtr: ;void _load_gdtr(const uint32_t GDT_LIMIT,struct GdtDescriptor* const GDT_ADDR);
    mov ax,[esp+4]
    mov [esp+6],ax
    lgdt [esp+6]
    jmp 0x10:reload_reg
reload_reg:
    mov ax,0x08
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov fs,ax
    mov gs,ax
    ret

_load_idtr: ;void _load_idtr(const uint32_t IDT_LIMIT,struct IdtDescriptor* const IDT_ADDR);
    mov ax,[esp+4]
    mov [esp+6],ax
    lidt [esp+6]
    ret

_out8: ;void out8(const uint32_t port,const uint32_t data);
    mov dx,[esp+4]
    mov al,[esp+8]
    out dx,al
    ret
