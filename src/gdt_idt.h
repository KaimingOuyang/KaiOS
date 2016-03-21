#ifndef GDT_IDT_H_INCLUDED
#define GDT_IDT_H_INCLUDED

#include <stdint.h>

#define GDT_LIMIT 0xffff
#define IDT_LIMIT 0x7ff
#define IDT_COUNT 0xff
#define OS_GDT_DATA_BASE 0
#define OS_GDT_CODE_BASE 0
#define OS_GDT_DATA_LIMIT 0xffffffff
#define OS_GDT_CODE_LIMIT 0xffffffff
#define KERNEL_CODE_SEGMENT 1001
#define KERNEL_DATA_SEGMENT 2001

#define TSS_AR 0x0089
#define IDT_AR 0x8e
#define OS_GDT_DATA_AR 0x0492
#define OS_GDT_CODE_AR 0x049a
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
void set_gdt_struct(uint32_t id, uint32_t limit, uint32_t base, uint16_t ar);

void idt_init();
void set_idt_struct(uint32_t id, uint32_t offset, uint16_t selector, uint8_t ar);

#endif // GDT_IDT_H_INCLUDED
