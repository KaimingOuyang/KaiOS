#ifndef GDT_IDT_H_INCLUDED
#define GDT_IDT_H_INCLUDED

#include <stdint.h>
struct GdtDescriptor{
    uint16_t limit_low,base_low;
    uint8_t base_mid,access_right;
    uint8_t ar_limit_high,base_high;
};

struct IdtDescriptor{
    uint16_t offset_low,selector;
    uint8_t zero,access_right;
    uint16_t offset_high;
};

void gdt_init();
void set_gdt_struct(struct GdtDescriptor* gdt_set,uint32_t limit,uint32_t base,uint16_t ar);

void idt_init();
void set_idt_struct(struct IdtDescriptor* idt_set,uint32_t offset,uint16_t selector,uint8_t ar);

void pic_init();
#endif // GDT_IDT_H_INCLUDED
