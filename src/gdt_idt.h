#ifndef GDT_IDT_H_INCLUDED
#define GDT_IDT_H_INCLUDED

#include <stdint.h>
struct GdtDescriptor{
    uint16_t limit_low,base_low;
    uint8_t base_mid,access_right;
    uint8_t ar_limit_high,base_high;
};

void gdt_init();
void set_gdt_struct(struct GdtDescriptor* gdt_set,uint32_t limit,uint32_t base,uint16_t ar);

#endif // GDT_IDT_H_INCLUDED
