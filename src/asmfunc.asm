
section .text
global _load_gdtr,_load_idtr,_tty_mode_switch
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
    push es
    push ds
    push ss
    mov ax,0x08
    mov ss,ax
    mov ds,ax
    mov es,ax
    call int21_keyboard
    pop ss
    pop ds
    pop es
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

_tty_mode_switch:
    mov edx,0
    mov eax,0
    mov dx,0x3c0
    mov al,0x10
    out dx,al
    mov ax,0x10a
    out dx,ax
    ret
