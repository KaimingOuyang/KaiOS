#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED

#include <tty.h>
#include <stdbool.h>
#include <gdt_idt.h>
#include <asmfunc.h>
#include <interrupts.h>
#include <fifo.h>

#if !defined(__i686__)
#error "This kernel needs to be compiled with a i686-elf compiler"
#endif

extern bool cap_lock;
extern bool num_lock;
extern bool scr_lock;
extern struct BufferPool common_buffer;

#endif // KERNEL_H_INCLUDED
