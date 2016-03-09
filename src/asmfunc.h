#ifndef ASMFUNC_H_INCLUDED
#define ASMFUNC_H_INCLUDED

#include <stdint.h>
#include <gdt_idt.h>

void _load_gdtr(const uint32_t limit,struct GdtDescriptor* const gdt_addr);

#endif // ASMFUNC_H_INCLUDED
