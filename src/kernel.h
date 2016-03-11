#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED

#include <tty.h>
#include <gdt_idt.h>
#include <interrupts.h>

#if !defined(__i686__)
#error "This kernel needs to be compiled with a i686-elf compiler"
#endif

#endif // KERNEL_H_INCLUDED
