section .text
global _load_gdtr,_load_idtr,_load_cr0,_set_cr0,_set_page_directory,_enable_paging
global _out8,_in8
global _asm_int21_keyboard,_asm_int20_timer,_asm_int26_io,_asm_int40_api
global _sti,_cli,_hlt
global _load_tr,_load_page_directory,_switch_task,_far_jmp,_start_app,_end_app
extern int21_keyboard,int20_timer,int26_io,int40_api

_start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);

		mov	eax,[esp+4]
		mov	ecx,[esp+8]
		mov	edx,[esp+12]
		mov	ebx,[esp+16]
		mov	ebp,[esp+20]
		;mov	[ebp  ],esp ; system esp
		;mov	[ebp+4],ss ; system ss
		cli
        mov	es,bx
		mov	ds,bx
		mov	fs,bx
		mov	gs,bx
        sti
        ;or		ecx,3
		;or		ebx,3

		;push edx ;app esp
		;push ebx ;app ss
        push ecx
        push eax

        retf
_end_app:
        cli
        mov ax,2001*8
        mov ds,ax
        mov es,ax
        mov fs,ax
        mov gs,ax
        ;mov ss,ax
        sti

        ret
        ;popad
		;jmp $
		;ret

_asm_int40_api:
    sti
    cmp ebx,3
    je once
    push ds
    push es
    pushad
once:
    pushad
    mov ax,2001*8
    mov ds,ax
    mov es,ax
    call int40_api
    add esp,32
    popad
    pop es
    pop ds
    iretd

_switch_task: ;void _switch_task(uint32_t eip,uint32_t cs);
_far_jmp:
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
    mov ax,ss
    mov ds,ax
    mov es,ax
    call int26_io
    pop es
    pop ds
    popad
    iretd

_asm_int21_keyboard: ;void _asm_int21_keyboard();
    pushad
    push ds
    push es
    mov ax,ss
    mov ds,ax
    mov es,ax
    call int21_keyboard
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
