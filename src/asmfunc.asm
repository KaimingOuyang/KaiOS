
section .text
global _load_gdtr

_load_gdtr: ;void _load_gdtr(const uint32_t limit,struct GdtDescriptor* const gdt_addr);
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
