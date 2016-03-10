#ifndef ASMFUNC_H_INCLUDED
#define ASMFUNC_H_INCLUDED

#include <stdint.h>
#include <gdt_idt.h>

void _load_gdtr(const uint32_t GDT_LIMIT,struct GdtDescriptor* const GDT_ADDR);
void _load_idtr(const uint32_t IDT_LIMIT,struct IdtDescriptor* const IDT_ADDR);

void _out8(const uint32_t port,const uint32_t data);
#endif // ASMFUNC_H_INCLUDED
