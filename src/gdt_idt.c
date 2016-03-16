#include <gdt_idt.h>
#include <asmfunc.h>
#include <memory.h>

const uint32_t GDT_LIMIT = 0xffff;
const uint32_t IDT_LIMIT = 0x7ff;
const uint16_t IDT_COUNT = 0xff;
const uint8_t IDT_AR = 0x8e;
const uint32_t OS_GDT_DATA_BASE = 0;
const uint32_t OS_GDT_CODE_BASE = 0;
const uint32_t OS_GDT_DATA_AR = 0x0492;
const uint32_t OS_GDT_CODE_AR = 0x049a;
const uint32_t OS_GDT_DATA_LIMIT = 0xffffffff;
const uint32_t OS_GDT_CODE_LIMIT = 0xffffffff;

struct GdtDescriptor* const GDT = (struct GdtDescriptor* const) 0x3f0000;
struct IdtDescriptor* const IDT = (struct IdtDescriptor* const) 0x3ef800;

void gdt_init() {
    set_gdt_struct(GDT, 0, 0, 0);
    set_gdt_struct(GDT + 1, OS_GDT_DATA_LIMIT, OS_GDT_DATA_BASE, OS_GDT_DATA_AR); // os data segment
    set_gdt_struct(GDT + 2, OS_GDT_CODE_LIMIT, OS_GDT_CODE_BASE, OS_GDT_CODE_AR); // os code segment
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

    gdt_set->ar_limit_high |= ar >> 4;
    return;
}

void idt_init() {

    memset(IDT, 0, sizeof(IDT)); // if I have to use sprintf, I must rewrite this line as for()
    _load_idtr(IDT_LIMIT, IDT);

    set_idt_struct(IDT + 0x21, (uint32_t)_asm_int21_keyboard, 0x10, IDT_AR);
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
