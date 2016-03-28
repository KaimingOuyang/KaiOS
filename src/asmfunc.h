#ifndef ASMFUNC_H_INCLUDED
#define ASMFUNC_H_INCLUDED

#include <stdint.h>
#include <gdt_idt.h>

void _cli();
void _sti();
void _hlt();
void _load_gdtr(const uint16_t GLIMIT,struct GdtDescriptor* const GDT_ADDR);
void _load_idtr(const uint16_t ILIMIT,struct IdtDescriptor* const IDT_ADDR);
void _tty_mode_switch();
void _out8(const uint16_t port,const uint8_t data);
uint8_t _in8(const uint32_t port);

void _asm_int20_timer();
void _asm_int21_keyboard();
void _asm_int26_io();

uint32_t _load_cr0();
void _set_cr0(uint32_t cr0);

void _set_page_directory(void* page_directory);
void _enable_paging();

void _load_tr(uint16_t seg);
uint32_t _load_page_directory();

void _switch_task(uint32_t eip,uint32_t cs);
#endif // ASMFUNC_H_INCLUDED
