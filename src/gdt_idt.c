#include <gdt_idt.h>
#include <asmfunc.h>
#include <memory.h>
#include <stddef.h>

/*
set_gdt_struct(GDT + 2002, 1 << 15, 0x400000, OS_GDT_DATA_AR ^ 0x0400); // BIOS image data 32KB
set_gdt_struct(GDT + 1002, 1 << 15, 0x400000, OS_GDT_CODE_AR ^ 0x0400); // BIOS image code 32KB,16bits code

set_gdt_struct(GDT + 2003, 1 << 15, 0x410000, OS_GDT_DATA_AR ^ 0x0400); // BIOS data field 32KB
set_gdt_struct(GDT + 2004, 1 << 16, 0xA0000, OS_GDT_DATA_AR ^ 0x0400); // BIOS 0xA0000 data 64KB
set_gdt_struct(GDT + 2005, 1 << 16, 0xB0000, OS_GDT_DATA_AR ^ 0x0400); // BIOS 0xB0000 data 64KB
set_gdt_struct(GDT + 2006, 1 << 15, 0xB8000, OS_GDT_DATA_AR ^ 0x0400); // BIOS 0xB8000 data 64KB
set_gdt_struct(GDT + 2007, 1 << 15, 0x420000, OS_GDT_DATA_AR ^ 0x0400); // BIOS stack data 64KB
*/

struct GdtDescriptor* GDT = (struct GdtDescriptor*) 0x3f0000;
struct IdtDescriptor* IDT = (struct IdtDescriptor*) 0x3ef800;

void gdt_init() {
    memset(GDT, 0, 1 << 16);
    set_gdt_struct(KERNEL_CODE_SEGMENT, OS_GDT_CODE_LIMIT, OS_GDT_CODE_BASE, OS_GDT_CODE_AR); // os code segment
    set_gdt_struct(KERNEL_DATA_SEGMENT, OS_GDT_DATA_LIMIT, OS_GDT_DATA_BASE, OS_GDT_DATA_AR); // os data segment

    _load_gdtr(GDT_LIMIT, GDT);
    return;
}

void set_gdt_struct(uint32_t id, uint32_t limit, uint32_t base, uint16_t ar) {

    GDT[id].limit_low = limit & 0xffff;
    GDT[id].ar_limit_high = (limit >> 16) & 0x000f;

    GDT[id].base_low = base & 0xffff;
    GDT[id].base_mid = (base >> 16) & 0x00ff;
    GDT[id].base_high = (base >> 24) & 0xff;

    GDT[id].access_right = ar & 0xff;
    ar = ar & 0x0f00;

    if(limit > 0xfffff)
        ar |= 1 << 11;

    GDT[id].ar_limit_high |= (uint8_t)(ar >> 4);
    return;
}

void idt_init() {
    memset(IDT, 0, 1 << 11); // if I have to use sprintf, I must rewrite this line as for()
    set_idt_struct(0x20, (uint32_t)_asm_int20_timer, KERNEL_CODE_SEGMENT * 8, IDT_AR); // timer
    set_idt_struct(0x21, (uint32_t)_asm_int21_keyboard, KERNEL_CODE_SEGMENT * 8, IDT_AR); // keyboard
    set_idt_struct(0x26, (uint32_t)_asm_int26_io, KERNEL_CODE_SEGMENT * 8, IDT_AR); // io
    set_idt_struct(0x40, (uint32_t)_asm_int40_api, KERNEL_CODE_SEGMENT * 8, IDT_AR); // api
    _load_idtr(IDT_LIMIT, IDT);
    return;
}

void set_idt_struct(uint32_t id, uint32_t offset, uint16_t selector, uint8_t ar) {

    IDT[id].offset_low = offset & 0xffff;
    IDT[id].offset_high = (offset >> 16) & 0xffff;

    IDT[id].selector = selector;

    IDT[id].zero = 0;

    IDT[id].access_right = ar;
    return;
}


