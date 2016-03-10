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

const uint32_t PIC0_ICW1 = 0x0020;
const uint32_t PIC0_ICW2 = 0x0021;
const uint32_t PIC0_ICW3 = 0x0021;
const uint32_t PIC0_ICW4 = 0x0021;

const uint32_t PIC1_ICW1 = 0x00a0;
const uint32_t PIC1_ICW2 = 0x00a1;
const uint32_t PIC1_ICW3 = 0x00a1;
const uint32_t PIC1_ICW4 = 0x00a1;

const uint32_t PIC0_IMR = 0x0021;
const uint32_t PIC1_IMR = 0x00a1;

const uint32_t PIC0_OCW2 = 0x0020;
const uint32_t PIC1_OCW2 = 0x00a0;


struct GdtDescriptor* const GDT = (struct GdtDescriptor* const) 0x3f0000;
struct IdtDescriptor* const IDT = (struct IdtDescriptor* const) 0x3ef800;

void gdt_init() {
    set_gdt_struct(GDT,0,0,0);
    set_gdt_struct(GDT+1,OS_GDT_DATA_LIMIT,OS_GDT_DATA_BASE,OS_GDT_DATA_AR); // os data segment
    set_gdt_struct(GDT+2,OS_GDT_CODE_LIMIT,OS_GDT_CODE_BASE,OS_GDT_CODE_AR); // os code segment
    _load_gdtr(GDT_LIMIT,GDT);
    return;
}

void set_gdt_struct(struct GdtDescriptor* gdt_set,uint32_t limit,uint32_t base,uint16_t ar) {

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


void idt_pic_init(){
    idt_init();
    pic_init();
}

void pic_init(){

    _out8(PIC0_IMR,0xff);
    _out8(PIC1_IMR,0xff);

    _out8(PIC0_ICW1,0x11);
    _out8(PIC0_ICW2,0x20);
    _out8(PIC0_ICW3,0x04);
    _out8(PIC0_ICW4,0x01);

    _out8(PIC1_ICW1,0x11);
    _out8(PIC1_ICW1,0x28);
    _out8(PIC1_ICW1,0x04);
    _out8(PIC1_ICW1,0x01);

    _out8(PIC0_IMR,0xfb);
    _out8(PIC1_IMR,0xff);
    return;
}

void idt_init(){

    memset(IDT,0,sizeof(IDT));
    _load_idtr(IDT_LIMIT,IDT);

}

void set_idt_struct(struct IdtDescriptor* idt_set,uint32_t offset,uint16_t selector,uint8_t ar){

    idt_set->offset_low = offset & 0xffff;
    idt_set->offset_high = (offset >> 16) & 0xffff;

    idt_set->selector = selector;

    idt_set->zero = 0;

    idt_set->access_right = ar;
}
