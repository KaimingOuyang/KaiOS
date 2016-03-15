
section .text
global _load_gdtr,_load_idtr,_load_cr0,_set_cr0,_set_page_directory,_enable_paging
global _out8,_in8
global _asm_int21_keyboard
global _sti,_cli,_hlt
extern int21_keyboard

_load_gdtr: ;void _load_gdtr(const uint32_t GDT_LIMIT,struct GdtDescriptor* const GDT_ADDR);
    mov ax,[esp+4]
    mov [esp+6],ax
    lgdt [esp+6]
    jmp 0x10:reload_reg ;reload cs register
reload_reg: ;reload all other segment registers
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

_in8: ;uint8_t _in8(const uint32 port);
    mov edx,[esp+4]
    mov eax,0
    in al,dx
    ret

_asm_int21_keyboard: ;void _asm_int21_keyboard();
    pushad
    push ds
    push ss
    mov ax,0x08
    mov ss,ax
    mov ds,ax
    call int21_keyboard
    pop ss
    pop ds
    popad
    iretd

_cli: ;void _cli();
    cli
    ret

_sti: ;void _sti();
    sti
    ret

_hlt: ;void _hlt();
    hlt
    ret

_load_cr0: ;uint32_t _load_cr0();
    mov eax,cr0
    ret

_set_cr0: ;void _set_cr0(uint32_t cr0);
    mov eax,[esp+4]
    mov cr0,eax
    ret

_set_page_directory: ;void _set_page_directory(void* page_directory);
    mov eax,[esp+4]
    mov cr3,eax
    ret

_enable_paging: ;void _enable_paging();
    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax
    ret
