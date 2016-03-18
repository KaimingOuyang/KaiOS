#include <gdt_idt.h>
#include <asmfunc.h>
#include <memory.h>
#include <tty.h>
#include <stddef.h>

struct GdtDescriptor* GDT = (struct GdtDescriptor*) 0x3f0000;
struct IdtDescriptor* IDT = (struct IdtDescriptor*) 0x3ef800;

void gdt_init() {
    memset(GDT, 0, 1 << 16);
    set_gdt_struct(GDT + KERNEL_CODE_SEGMENT, OS_GDT_CODE_LIMIT, OS_GDT_CODE_BASE, OS_GDT_CODE_AR); // os code segment
    set_gdt_struct(GDT + KERNEL_DATA_SEGMENT, OS_GDT_DATA_LIMIT, OS_GDT_DATA_BASE, OS_GDT_DATA_AR); // os data segment

    /*
    set_gdt_struct(GDT + 2002, 1 << 15, 0x400000, OS_GDT_DATA_AR ^ 0x0400); // BIOS image data 32KB
    set_gdt_struct(GDT + 1002, 1 << 15, 0x400000, OS_GDT_CODE_AR ^ 0x0400); // BIOS image code 32KB,16bits code

    set_gdt_struct(GDT + 2003, 1 << 15, 0x410000, OS_GDT_DATA_AR ^ 0x0400); // BIOS data field 32KB
    set_gdt_struct(GDT + 2004, 1 << 16, 0xA0000, OS_GDT_DATA_AR ^ 0x0400); // BIOS 0xA0000 data 64KB
    set_gdt_struct(GDT + 2005, 1 << 16, 0xB0000, OS_GDT_DATA_AR ^ 0x0400); // BIOS 0xB0000 data 64KB
    set_gdt_struct(GDT + 2006, 1 << 15, 0xB8000, OS_GDT_DATA_AR ^ 0x0400); // BIOS 0xB8000 data 64KB
    set_gdt_struct(GDT + 2007, 1 << 15, 0x420000, OS_GDT_DATA_AR ^ 0x0400); // BIOS stack data 64KB
    */
    _load_gdtr(GDT_LIMIT, GDT);
    return;
}

void set_gdt_struct(struct GdtDescriptor* gdt_set, uint32_t limit, uint32_t base, uint16_t ar) {

    gdt_set->limit_low = limit & 0xffff;
    gdt_set->ar_limit_high = (limit >> 16) & 0x000f;

    gdt_set->base_low = base & 0xffff;
    gdt_set->base_mid = (base >> 16) & 0x00ff;
    gdt_set->base_high = (base >> 24) & 0xff;

    gdt_set->access_right = ar & 0xff;
    ar = ar & 0x0f00;

    if(limit > 0xfffff)
        ar |= 1 << 11;

    gdt_set->ar_limit_high |= (uint8_t)(ar >> 4);
    return;
}

void idt_init() {
    memset(IDT, 0, 1 << 11); // if I have to use sprintf, I must rewrite this line as for()
    set_idt_struct(IDT + 0x20, (uint32_t)_asm_int20_timer,KERNEL_CODE_SEGMENT * 8, IDT_AR); // timer
    set_idt_struct(IDT + 0x21, (uint32_t)_asm_int21_keyboard, KERNEL_CODE_SEGMENT * 8, IDT_AR); // keyboard
    _load_idtr(IDT_LIMIT, IDT);
    return;
}

void set_idt_struct(struct IdtDescriptor* idt_set, uint32_t offset, uint16_t selector, uint8_t ar) {

    idt_set->offset_low = offset & 0xffff;
    idt_set->offset_high = (offset >> 16) & 0xffff;

    idt_set->selector = selector;

    idt_set->zero = 0;

    idt_set->access_right = ar;
    return;
}


