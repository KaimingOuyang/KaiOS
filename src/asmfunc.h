#ifndef ASMFUNC_H_INCLUDED
#define ASMFUNC_H_INCLUDED

#include <stdint.h>
#include <gdt_idt.h>

void _cli();
void _sti();
void _hlt();
void _load_gdtr(const uint32_t GDT_LIMIT,struct GdtDescriptor* const GDT_ADDR);
void _load_idtr(const uint32_t IDT_LIMIT,struct IdtDescriptor* const IDT_ADDR);

void _out8(const uint32_t port,const uint32_t data);
uint8_t _in8(const uint32_t port);

void _asm_int21_keyboard();


#endif // ASMFUNC_H_INCLUDED
