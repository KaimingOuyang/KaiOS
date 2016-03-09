#include <gdt_idt.h>
#include <asmfunc.h>

const uint32_t gdt_limit = 0xffff;
struct GdtDescriptor* const gdt = (struct GdtDescriptor* const) 0x3f0000;
//const * idt = (uint32_t*) 0x3ef800;

void gdt_init() {
    set_gdt_struct(gdt,0,0,0);
    set_gdt_struct(gdt+1,0xffffffff,0,0x0492); // os data segment
    set_gdt_struct(gdt+2,0xffffffff,0,0x049a); // os code segment
    _load_gdtr(gdt_limit,gdt);
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
