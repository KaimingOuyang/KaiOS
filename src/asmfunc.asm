section .text
global _load_gdtr,_load_idtr,_load_cr0,_set_cr0,_set_page_directory,_enable_paging
global _out8,_in8
global _asm_int21_keyboard,_asm_int20_timer,_asm_int26_io
global _sti,_cli,_hlt
global _load_tr,_load_page_directory,_switch_task
extern int21_keyboard,int20_timer,int26_io

_switch_task: ;void _switch_task(uint32_t eip,uint32_t cs);
    jmp far [esp+4]
    ret

_load_tr: ;void _load_tr(uint16_t seg);
    ltr [esp+4]
    ret

_load_gdtr: ;void _load_gdtr(const uint32_t GDT_LIMIT,struct GdtDescriptor* const GDT_ADDR);
    mov ax,[esp+4]
    mov [esp+6],ax
    lgdt [esp+6]
    jmp 1001*8:reload_reg ;reload cs register
reload_reg: ;reload all other segment registers
    mov ax,2001*8
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

_asm_int26_io: ;void _asm_int26_io();
    pushad
    push ds
    push es
    push ss
    mov ax,2001*8
    mov ss,ax
    mov ds,ax
    mov es,ax
    call int26_io
    pop ss
    pop es
    pop ds
    popad
    iretd

_asm_int21_keyboard: ;void _asm_int21_keyboard();
    pushad
    push ds
    push es
    push ss
    mov ax,2001*8
    mov ss,ax
    mov ds,ax
    mov es,ax
    call int21_keyboard
    pop ss
    pop es
    pop ds
    popad
    iretd

_asm_int20_timer: ;void _asm_int20_timer();
    pushad
    push ds
    push es
    mov ax,ss
    mov ds,ax
    mov es,ax
    call int20_timer
    pop es
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

_load_page_directory: ;uint32_t _load_page_directory();
    mov eax,cr3
    ret
